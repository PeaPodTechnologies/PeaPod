#include <Arduino.h>
#include <unity.h>

#include <I2CIP.h>

#include <menu.h>

#define PEAPOD_DEBUG_MODULE_NUM 0
#define PEAPOD_DEBUG_MODULE_INTERVALS 1 // Uncomment to enable chronograph callbacks on debug module
#define PEAPOD_DEBUG_SHT45_DELTA 1000

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
}

void test_menu_update(void) {
  i2cip_errorlevel_t errlev_menu = PeaPod::Menu::update();
  TEST_ASSERT_EQUAL_UINT8_MESSAGE(I2CIP_ERR_NONE, errlev_menu, "Menu Update");
}

unsigned long i = 0;

void loop() {

  PeaPod::cycle.set(PeaPod::cycle.get()++);
  
  RUN_TEST(test_menu_update);
  delay(100);

  if(i++ > 1000) {
    UNITY_END();
    while(true) delay(1000);
  }
}