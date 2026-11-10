#include <Arduino.h>
#include <unity.h>

#include <I2CIP.h>

#include <menu.h>

#define PEAPOD_DEBUG_MODULE_NUM 0
#define PEAPOD_DEBUG_MODULE_INTERVALS 1 // Uncomment to enable chronograph callbacks on debug module
#define PEAPOD_DEBUG_SHT45_DELTA 1000
#define PEAPOD_DEBUG_CONFIG_STRING {"{\"list\":true}"}
#define PEAPOD_DEBUG_INTERVAL_STRING {"{\"interval\":\"i_read_sht45\",\"delta\":500}"} // Find interval, change delta
#define PEAPOD_DEBUG_EVENT_STRING_ON {"{\"flag\":\"debug\",\"delta\":20000}"} // Link new flag event, set true
#define PEAPOD_DEBUG_EVENT_STRING_OFF {"{\"flag\":\"debug\",\"delta\":30000,\"invert\":true}"} // Link new flag event, set false

using namespace I2CIP;

unsigned x = 0;
int32_t pos = 0;
bool button = false;

FSM::Variable air_temperature;
FSM::Variable air_humidity;

void callback_temperature(bool _, const FSM::Number& v) {
  DebugJson::telemetry(millis(), (double)v, air_temperature.getKey());
}

void callback_humidity(bool _, const FSM::Number& v) {
  DebugJson::telemetry(millis(), (double)v, air_humidity.getKey());
}

class PeaPodModuleDebug : public PeaPod::PeaPodModule {
  private:
    FSM::IntervalCallback* interval_sht45 = nullptr;
    FSM::Flag flag_debug = FSM::Flag("debug");
  public:
    PeaPodModuleDebug() : PeaPodModule(PEAPOD_DEBUG_MODULE_NUM) {
      #ifdef PEAPOD_DEBUG_MODULE_INTERVALS
        interval_sht45 = FSM::Chronos.addInterval("i_read_sht45", PEAPOD_DEBUG_SHT45_DELTA, PeaPodModuleDebug::callback_sht45_mean<PEAPOD_DEBUG_MODULE_NUM>);

        registerInterval(interval_sht45); // NOT LOCKED
      #endif

      air_temperature.addConditional(FSM::CMP_NEQ, FSM::notanumber, callback_temperature);
      air_humidity.addConditional(FSM::CMP_NEQ, FSM::notanumber, callback_humidity);

      registerVariable(&air_temperature, true); // LOCKED
      registerVariable(&air_humidity, true); // LOCKED

      flag_debug.addLatchingConditional(true, false, &PeaPod::controlPin<13>);
      // flag_debug.addLatchingConditional(true, false, debug_print);
      registerFlag(&flag_debug); // NOT LOCKED
    }
  
  template <unsigned char M> static void callback_sht45_mean(bool _ = true, const FSM::fsm_timestamp_t& __ = 0) {
    if(M < 0 || M > I2CIP_MUX_COUNT || I2CIP::modules[M] == nullptr) return;

    // If the module is not in an errored state,
    if(I2CIP::errlev[M] == I2CIP_ERR_NONE) {
      // Get SHT45 device group
      I2CIP::DeviceGroup* dg_sht45 = I2CIP::modules[M]->operator[]("SHT45");

      // If we have devices,
      if(dg_sht45 != nullptr && dg_sht45->getNumDevices() > 0) {

        // Read from all devices and take a mean
        state_sht45_t th = {0.0f, 0.0f}; uint8_t c = 0;
        for(uint8_t i = 0; i < dg_sht45->getNumDevices(); i++) {
          SHT45* d = (SHT45*)(dg_sht45->getDevice(i));
          if(d == nullptr) continue;

          i2cip_errorlevel_t errlev_sht45 = I2CIP::modules[M]->operator()<SHT45>(d->getFQA(), true, _i2cip_args_io_default, DebugJsonBreakpoints);
          if(errlev_sht45 != I2CIP_ERR_NONE) continue;

          th.temperature += d->getCache().temperature;
          th.humidity += d->getCache().humidity;
          c++;
        }

        // If readings were taken,
        if(c > 0) {
          // Set variables according to averages
          air_temperature.set(th.temperature / c);
          air_humidity.set(th.humidity / c);
        }
      }
    }
  }
};

void test_ping_encoder(void) {
  i2cip_errorlevel_t errlev = PeaPod::notamodule.operator()<RotaryEncoder>(PeaPod::Menu::fqa_encoder, false);
  TEST_ASSERT_EQUAL_UINT8_MESSAGE(I2CIP_ERR_NONE, errlev, "Rotary Encoder Ping");
}

void test_ping_lcd(void) {
  i2cip_errorlevel_t errlev = PeaPod::notamodule.operator()<MCP23008>(PeaPod::Menu::fqa_lcd, false);
  TEST_ASSERT_EQUAL_UINT8_MESSAGE(I2CIP_ERR_NONE, errlev, "LCD Ping");
}

void test_ping_sevenseg(void) {
  i2cip_errorlevel_t errlev = PeaPod::notamodule.operator()<HT16K33>(PeaPod::fqa_sevenseg, true, _i2cip_args_io_default);
  TEST_ASSERT_EQUAL_UINT8_MESSAGE(I2CIP_ERR_NONE, errlev, "7-Segment Display");
}

void test_menu_init(void) {
  i2cip_errorlevel_t errlev_init = PeaPod::Menu::initialize();
  TEST_ASSERT_EQUAL_UINT8_MESSAGE(I2CIP_ERR_NONE, errlev_init, "Menu Initialization");
}

void test_debug_init(void) {
  PeaPod::callback_module<PEAPOD_DEBUG_MODULE_NUM, PeaPodModuleDebug>();
  PeaPod::registerCallbacks();
  TEST_ASSERT_NOT_NULL_MESSAGE(I2CIP::modules[PEAPOD_DEBUG_MODULE_NUM], "Debug Module Initialization");
}

void test_config(void) {
  const char* configString = PEAPOD_DEBUG_CONFIG_STRING;

  JsonDocument config;

  DeserializationError err = deserializeJson(config, configString);

  TEST_ASSERT_FALSE_MESSAGE((bool)err, (String("JSON command deserialization failed: ") + configString).c_str());

  DebugJson::StringWriter out;
  PeaPod::configRouter(config.as<JsonObject>(), out);
  String str = out.operator String();
  out.flush();
  TEST_ASSERT_TRUE_MESSAGE(str.length() > 0, "JSON command output is empty");
  
  JsonDocument output = JsonDocument();
  DeserializationError error = deserializeJson(output, str);
  TEST_ASSERT_FALSE_MESSAGE((bool)error, ("JSON command output deserialization failed: " + str).c_str());

  TEST_ASSERT_TRUE_MESSAGE(output.is<JsonObject>(), "JSON command output is not an object");

  TEST_ASSERT_TRUE_MESSAGE(output["type"].is<const char*>(), "JSON command output does not contain 'type' key");
  TEST_ASSERT_EQUAL_STRING_MESSAGE("config", output["type"].as<const char*>(), "JSON command output 'type' is not 'config'");

  TEST_ASSERT_TRUE_MESSAGE(output["timestamp"].is<unsigned long>(), "JSON command output 'timestamp' is not an unsigned long");

  // TEST_ASSERT_TRUE_MESSAGE(output["id"].is<const char*>(), "JSON command output 'id' is not a const char*");
  // TEST_ASSERT_EQUAL_STRING_MESSAGE(I2CIP_TEST_COMMAND_OUTPUT_ID, output["id"].as<const char*>(), "JSON command output 'id' is not '" I2CIP_TEST_COMMAND_OUTPUT_ID "'");

  // TEST_ASSERT_TRUE_MESSAGE(output["fqa"].is<uint16_t>(), "JSON command output 'fqa' is not a uint16_t");
  // TEST_ASSERT_EQUAL_UINT16_MESSAGE(I2CIP_TEST_FQA, output["fqa"].as<uint16_t>(), "JSON command output 'fqa' is not '" STR(I2CIP_TEST_FQA) "'");

  TEST_ASSERT_TRUE_MESSAGE(output["errlev"].is<int>(), "JSON command output 'errlev' is not an int");
  TEST_ASSERT_EQUAL_INT_MESSAGE(I2CIP_ERR_NONE, output["errlev"].as<int>(), "JSON command output 'errlev' is not 0");

  TEST_PASS_MESSAGE(("JSON command output generated: " + str).c_str());
}

void test_interval(void) {
  const char* intervalString = PEAPOD_DEBUG_INTERVAL_STRING;

  JsonDocument interval;

  DeserializationError err = deserializeJson(interval, intervalString);

  TEST_ASSERT_FALSE_MESSAGE((bool)err, (String("JSON command deserialization failed: ") + intervalString).c_str());

  DebugJson::StringWriter out;
  PeaPod::intervalRouter(interval.as<JsonObject>(), out);
  String str = out.operator String();
  out.flush();
  TEST_ASSERT_TRUE_MESSAGE(str.length() > 0, "JSON command output is empty");
  
  JsonDocument output = JsonDocument();
  DeserializationError error = deserializeJson(output, str);
  TEST_ASSERT_FALSE_MESSAGE((bool)error, ("JSON command output deserialization failed: " + str).c_str());

  TEST_ASSERT_TRUE_MESSAGE(output.is<JsonObject>(), "JSON command output is not an object");

  TEST_ASSERT_TRUE_MESSAGE(output["type"].is<const char*>(), "JSON command output does not contain 'type' key");
  TEST_ASSERT_EQUAL_STRING_MESSAGE("interval", output["type"].as<const char*>(), "JSON command output 'type' is not 'interval'");

  TEST_ASSERT_TRUE_MESSAGE(output["timestamp"].is<unsigned long>(), "JSON command output 'timestamp' is not an unsigned long");

  // TEST_ASSERT_TRUE_MESSAGE(output["id"].is<const char*>(), "JSON command output 'id' is not a const char*");
  // TEST_ASSERT_EQUAL_STRING_MESSAGE(I2CIP_TEST_COMMAND_OUTPUT_ID, output["id"].as<const char*>(), "JSON command output 'id' is not '" I2CIP_TEST_COMMAND_OUTPUT_ID "'");

  // TEST_ASSERT_TRUE_MESSAGE(output["fqa"].is<uint16_t>(), "JSON command output 'fqa' is not a uint16_t");
  // TEST_ASSERT_EQUAL_UINT16_MESSAGE(I2CIP_TEST_FQA, output["fqa"].as<uint16_t>(), "JSON command output 'fqa' is not '" STR(I2CIP_TEST_FQA) "'");

  TEST_ASSERT_TRUE_MESSAGE(output["errlev"].is<int>(), "JSON command output 'errlev' is not an int");
  TEST_ASSERT_EQUAL_INT_MESSAGE(I2CIP_ERR_NONE, output["errlev"].as<int>(), "JSON command output 'errlev' is not 0");

  TEST_PASS_MESSAGE(("JSON command output generated: " + str).c_str());
}

void test_event_on(void) {
  const char* eventString = PEAPOD_DEBUG_EVENT_STRING_ON;

  JsonDocument event;

  DeserializationError err = deserializeJson(event, eventString);

  TEST_ASSERT_FALSE_MESSAGE((bool)err, (String("JSON command deserialization failed: ") + eventString).c_str());

  DebugJson::StringWriter out;
  PeaPod::eventRouter(event.as<JsonObject>(), out);
  String str = out.operator String();
  out.flush();
  TEST_ASSERT_TRUE_MESSAGE(str.length() > 0, "JSON command output is empty");
  
  JsonDocument output = JsonDocument();
  DeserializationError error = deserializeJson(output, str);
  TEST_ASSERT_FALSE_MESSAGE((bool)error, ("JSON command output deserialization failed: " + str).c_str());

  TEST_ASSERT_TRUE_MESSAGE(output.is<JsonObject>(), "JSON command output is not an object");

  TEST_ASSERT_TRUE_MESSAGE(output["type"].is<const char*>(), "JSON command output does not contain 'type' key");
  TEST_ASSERT_EQUAL_STRING_MESSAGE("event", output["type"].as<const char*>(), "JSON command output 'type' is not 'event'");

  TEST_ASSERT_TRUE_MESSAGE(output["timestamp"].is<unsigned long>(), "JSON command output 'timestamp' is not an unsigned long");

  // TEST_ASSERT_TRUE_MESSAGE(output["id"].is<const char*>(), "JSON command output 'id' is not a const char*");
  // TEST_ASSERT_EQUAL_STRING_MESSAGE(I2CIP_TEST_COMMAND_OUTPUT_ID, output["id"].as<const char*>(), "JSON command output 'id' is not '" I2CIP_TEST_COMMAND_OUTPUT_ID "'");

  // TEST_ASSERT_TRUE_MESSAGE(output["fqa"].is<uint16_t>(), "JSON command output 'fqa' is not a uint16_t");
  // TEST_ASSERT_EQUAL_UINT16_MESSAGE(I2CIP_TEST_FQA, output["fqa"].as<uint16_t>(), "JSON command output 'fqa' is not '" STR(I2CIP_TEST_FQA) "'");

  TEST_ASSERT_TRUE_MESSAGE(output["errlev"].is<int>(), "JSON command output 'errlev' is not an int");
  TEST_ASSERT_EQUAL_INT_MESSAGE(I2CIP_ERR_NONE, output["errlev"].as<int>(), "JSON command output 'errlev' is not 0");

  TEST_PASS_MESSAGE(("JSON command output generated: " + str).c_str());
}

void test_event_off(void) {
  const char* eventString = PEAPOD_DEBUG_EVENT_STRING_OFF;

  JsonDocument event;

  DeserializationError err = deserializeJson(event, eventString);

  TEST_ASSERT_FALSE_MESSAGE((bool)err, (String("JSON command deserialization failed: ") + eventString).c_str());

  DebugJson::StringWriter out;
  PeaPod::eventRouter(event.as<JsonObject>(), out);
  String str = out.operator String();
  out.flush();
  TEST_ASSERT_TRUE_MESSAGE(str.length() > 0, "JSON command output is empty");
  
  JsonDocument output = JsonDocument();
  DeserializationError error = deserializeJson(output, str);
  TEST_ASSERT_FALSE_MESSAGE((bool)error, ("JSON command output deserialization failed: " + str).c_str());

  TEST_ASSERT_TRUE_MESSAGE(output.is<JsonObject>(), "JSON command output is not an object");

  TEST_ASSERT_TRUE_MESSAGE(output["type"].is<const char*>(), "JSON command output does not contain 'type' key");
  TEST_ASSERT_EQUAL_STRING_MESSAGE("event", output["type"].as<const char*>(), "JSON command output 'type' is not 'event'");

  TEST_ASSERT_TRUE_MESSAGE(output["timestamp"].is<unsigned long>(), "JSON command output 'timestamp' is not an unsigned long");

  // TEST_ASSERT_TRUE_MESSAGE(output["id"].is<const char*>(), "JSON command output 'id' is not a const char*");
  // TEST_ASSERT_EQUAL_STRING_MESSAGE(I2CIP_TEST_COMMAND_OUTPUT_ID, output["id"].as<const char*>(), "JSON command output 'id' is not '" I2CIP_TEST_COMMAND_OUTPUT_ID "'");

  // TEST_ASSERT_TRUE_MESSAGE(output["fqa"].is<uint16_t>(), "JSON command output 'fqa' is not a uint16_t");
  // TEST_ASSERT_EQUAL_UINT16_MESSAGE(I2CIP_TEST_FQA, output["fqa"].as<uint16_t>(), "JSON command output 'fqa' is not '" STR(I2CIP_TEST_FQA) "'");

  TEST_ASSERT_TRUE_MESSAGE(output["errlev"].is<int>(), "JSON command output 'errlev' is not an int");
  TEST_ASSERT_EQUAL_INT_MESSAGE(I2CIP_ERR_NONE, output["errlev"].as<int>(), "JSON command output 'errlev' is not 0");

  TEST_PASS_MESSAGE(("JSON command output generated: " + str).c_str());
}

void setup() {
  Serial.begin(115200); 

  delay(2000);
  
  UNITY_BEGIN();
  
  delay(1000);

  RUN_TEST(test_ping_encoder);
  delay(1000);
  RUN_TEST(test_ping_lcd);
  delay(1000);
  RUN_TEST(test_ping_sevenseg);
  delay(1000);
  RUN_TEST(test_menu_init);
  delay(1000);

  RUN_TEST(test_debug_init);
  delay(1000);

  RUN_TEST(test_config);
  delay(1000);
  RUN_TEST(test_interval);
  delay(1000);
  RUN_TEST(test_event_on);
  delay(1000);
  RUN_TEST(test_event_off);
  delay(1000);
}

void test_menu_update(void) {
  i2cip_errorlevel_t errlev_menu = PeaPod::Menu::update();
  TEST_ASSERT_EQUAL_UINT8_MESSAGE(I2CIP_ERR_NONE, errlev_menu, "Menu Update");
}

unsigned long i = 0;

void loop() {

  PeaPod::cycle.set(PeaPod::cycle.get() + FSM::Number(1, false, false));
  
  RUN_TEST(test_menu_update);
  delay(100);

  if(i++ > 1000) {
    UNITY_END();
    while(true) delay(1000);
  }
}