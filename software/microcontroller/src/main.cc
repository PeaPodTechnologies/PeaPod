#ifndef UNIT_TEST
#define UNIT_TEST 1
#define IS_MAIN 1

#include <Arduino.h>

#include <peapod.h>

#include <DebugJson.h>

#define HEARTBEAT_DELAY 5000

void callback_heartbeat(bool _, const FSM::fsm_timestamp_t& __);
void callback_module(bool _ = true, const FSM::fsm_timestamp_t& __ = 0);
void callback_sht45(bool _, const FSM::fsm_timestamp_t& __);
void callback_temperature(bool _, const FSM::Number& v);
void callback_humidity(bool _, const FSM::Number& v);

using namespace PeaPod;
using namespace I2CIP;

// SETUP GLOBALS

i2cip_fqa_t fqa_sht45 = createFQA(PEAPOD_WIRENUM, PEAPOD_MODULENUM, 0, I2CIP_SHT45_ADDRESS);
// i2cip_fqa_t fqa_pca9685 = createFQA(PEAPOD_WIRENUM, PEAPOD_MODULENUM, 1, I2CIP_PCA9685_ADDRESS);
// i2cip_fqa_t fqa_lcd = createFQA(PEAPOD_WIRENUM, PEAPOD_MODULENUM, 1, I2CIP_JHD1313_ADDRESS);
// i2cip_fqa_t fqa_rotary = createFQA(PEAPOD_WIRENUM, PEAPOD_MODULENUM, 0, I2CIP_SEESAW_ADDRESS);
// i2cip_fqa_t fqa_nunchuck = createFQA(PEAPOD_WIRENUM, PEAPOD_MODULENUM, 0, I2CIP_NUNCHUCK_ADDRESS);

FSM::Variable cycle(FSM::Number(0, false, false), "cycle");
FSM::Variable temperature = FSM::Variable(FSM::Number(), "air-temperature");
FSM::Variable humidity = FSM::Variable(FSM::Number(), "air-humidity");
const FSM::Number notanumber = FSM::Number();

void setup(void) {
  // 0. Builtin LED Pinmode; Serial Begin

  pinMode(LED_BUILTIN, OUTPUT);
  Serial.begin(115200);
  while(!Serial) { digitalWrite(LED_BUILTIN, HIGH); delay(100); digitalWrite(LED_BUILTIN, LOW); delay(100); }

  // Instantiate module ahead of time
  callback_module();

  // Load Devices
  modules[PEAPOD_MODULENUM]->operator()<HT16K33>(I2CIP::sevenSegmentFQA, true, _i2cip_args_io_default, DebugJsonBreakpoints);
  // modules[PEAPOD_MODULENUM]->operator()<PCA9685>(fqa_pca9685, false, _i2cip_args_io_default, DebugJsonBreakpoints);
  modules[PEAPOD_MODULENUM]->operator()<SHT45>(fqa_sht45, true, _i2cip_args_io_default, DebugJsonBreakpoints);
  // modules[PEAPOD_MODULENUM]->operator()<JHD1313>(fqa_lcd, true, _i2cip_args_io_default, DebugJsonBreakpoints);
  // modules[PEAPOD_MODULENUM]->operator()<Seesaw>(fqa_rotary, true, _i2cip_args_io_default, DebugJsonBreakpoints);

  humidity.addConditional(FSM::CMP_NEQ, notanumber, callback_humidity);
  temperature.addConditional(FSM::CMP_NEQ, notanumber, callback_temperature);

  FSM::Chronos.addInterval(HEARTBEAT_DELAY, callback_heartbeat);
  FSM::Chronos.addInterval(100, callback_module);
  FSM::Chronos.addInterval(250, callback_sht45);

}

// LOOP GLOBALS

unsigned long last = 0;
uint32_t fps = 0;

void loop(void) {

  // I2CIP_DEBUG_SERIAL.println(I2CIP::devicetree.toString());
  // I2CIP_DEBUG_SERIAL.println(modules[PEAPOD_MODULENUM]->toString());

  cycle.set(cycle.get() + FSM::Number(1, false, false));

  last = millis();
  FSM::Chronos.set(last); // Update chronograph and do event/interval conditionals & callbacks(?)

  while(Serial.available() > 0) { // With baud 115200, this should not block
    DebugJson::update(Serial, I2CIP::commandRouter);
  }

  #ifdef CYCLE_DELAY
  delay(CYCLE_DELAY);
  #endif

  unsigned long delta = millis() - last;
  fps += 1000.f / max(1.f, (float)delta);
  fps /= 2;
}

// HELPER FUNCTIONS

void callback_heartbeat(bool _, const FSM::fsm_timestamp_t& __) {
  DebugJson::heartbeat(millis(), Serial);
  DebugJson::revision(I2CIP_REVISION, Serial);
  DebugJson::telemetry(millis(), fps, "fps", Serial);
  DebugJson::telemetry(millis(), (unsigned)cycle.get(), "cycle", Serial);
}

void callback_module(bool _, const FSM::fsm_timestamp_t& __) {
  if(I2CIP::MUX::pingMUX(PEAPOD_WIRENUM, PEAPOD_MODULENUM)) {
    if(I2CIP::modules[PEAPOD_MODULENUM] == nullptr) {
      I2CIP::modules[PEAPOD_MODULENUM] = new PeaPodModule();

      // First Module - Add HT16K33
      I2CIP::modules[PEAPOD_MODULENUM]->operator()<HT16K33>(I2CIP::sevenSegmentFQA, true, _i2cip_args_io_default, NullStream);
    }

    I2CIP::errlev[PEAPOD_MODULENUM] = I2CIP::modules[PEAPOD_MODULENUM]->operator()();
  } else {
    I2CIP::errlev[PEAPOD_MODULENUM] = I2CIP_ERR_HARD;
  }

  for(uint8_t m = 0; m < I2CIP_MUX_COUNT; m++) {
    if(I2CIP::modules[m] != nullptr && I2CIP::errlev[m] == I2CIP_ERR_HARD) {
      delete I2CIP::modules[m];
      I2CIP::modules[m] = nullptr;
    }
  }
}

void callback_sht45(bool _, const FSM::fsm_timestamp_t& __)  {
  if(I2CIP::errlev[PEAPOD_MODULENUM] == I2CIP_ERR_NONE) {
    I2CIP::DeviceGroup* dg_sht45 = modules[PEAPOD_MODULENUM]->operator[]("SHT45");
    if(dg_sht45 != nullptr && dg_sht45->getNumDevices() > 0) {
      // AVERAGES
      state_sht45_t th = {0.0f, 0.0f}; uint8_t c = 0;
      for(uint8_t i = 0; i < dg_sht45->getNumDevices(); i++) {
        SHT45* d = (SHT45*)(dg_sht45->getDevice(i));
        if(d == nullptr) continue;

        i2cip_errorlevel_t errlev_sht45 = modules[PEAPOD_MODULENUM]->operator()<SHT45>(d->getFQA(), true, _i2cip_args_io_default, DebugJsonBreakpoints);
        if(errlev_sht45 != I2CIP_ERR_NONE) continue;

        th.temperature += d->getCache().temperature;
        th.humidity += d->getCache().humidity;
        c++;

        // DebugJson::telemetry(d->getLastRX(), d->getCache().temperature, "air-temperature");
        // DebugJson::telemetry(d->getLastRX(), d->getCache().humidity, "air-humidity");
      }

      if(c > 0) {

        th.temperature /= c; th.humidity /= c;

        // seg_data.f = th.temperature;
        // seg_mode = SEG_1F;

        temperature.set(th.temperature);
        humidity.set(th.humidity);

      }
    }
}
}

void callback_temperature(bool _, const FSM::Number& v) {
  DebugJson::telemetry(millis(), (double)v, "air-temperature");

  i2cip_ht16k33_mode_t seg_mode = SEG_1F;
  i2cip_ht16k33_data_t seg_data = { .f = (float)v };
  i2cip_args_io_t seg_args = { .a = nullptr, .s = &seg_data, .b = &seg_mode };

  I2CIP::modules[PEAPOD_MODULENUM]->operator()<HT16K33>(I2CIP::sevenSegmentFQA, true, seg_args, DebugJsonBreakpoints);
}

void callback_humidity(bool _, const FSM::Number& v) {
  DebugJson::telemetry(millis(), (double)v, "air-humidity");
}

#endif