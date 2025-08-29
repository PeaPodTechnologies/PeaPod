#ifndef UNIT_TEST
#define UNIT_TEST 1
#define IS_MAIN 1

#include <Arduino.h>

#include <I2CIP.h>

#include <peapod.h>

#define HEARTBEAT_DELAY 5000
#define LCD_REFRESH_MAX 1000
#define RGB_REFRESH_MAX 1
#define EPSILON_TEMPERATURE 0.5f
#define EPSILON_HUMIDITY 2.0f // 0.11f

// PeaPod Stuff
#define PEAPOD_PIN_TOGGLE 33 // Multipurpose; Interpreted input from rotary button (Debounced; Toggle)
#define PEAPOD_PIN_OUT 32 // Multipurpose; Direct input from rotary button
#define PEAPOD_PIN_PWM1 15 // Multipurpose; Interpreted output from rotary knob (0-360 -> 0-255)
#define PEAPOD_PIN_PWM2 14 // Multipurpose; Interpreted output from rotary knob (0-360 -> 0-255)
#define DURATION_WATERING   2000     // 10 seconds every...
#define PERIOD_WATERING     10000   //  ...30 minutes
#define DURATION_LIGHTING   (TWENTYFOURHRS_MILLIS*1/2) // 12:12 hrs
#define PIN_WATERING        PEAPOD_PIN_OUT

using namespace PeaPod;
using namespace I2CIP;

i2cip_fqa_t fqa_sht45 = createFQA(PEAPOD_WIRENUM, PEAPOD_MODULENUM, 0, I2CIP_SHT45_ADDRESS);
// i2cip_fqa_t fqa_pca9685 = createFQA(PEAPOD_WIRENUM, PEAPOD_MODULENUM, 1, I2CIP_PCA9685_ADDRESS);
i2cip_fqa_t fqa_lcd = createFQA(PEAPOD_WIRENUM, PEAPOD_MODULENUM, 1, I2CIP_JHD1313_ADDRESS);
i2cip_fqa_t fqa_rotary = createFQA(PEAPOD_WIRENUM, PEAPOD_MODULENUM, 0, I2CIP_SEESAW_ADDRESS);
i2cip_fqa_t fqa_nunchuck = createFQA(PEAPOD_WIRENUM, PEAPOD_MODULENUM, 0, I2CIP_NUNCHUCK_ADDRESS);

void setup(void) {
  // 0. Builtin LED Pinmode; Serial Begin

  pinMode(LED_BUILTIN, OUTPUT);
  Serial.begin(115200);
  while(!Serial) { digitalWrite(LED_BUILTIN, HIGH); delay(100); digitalWrite(LED_BUILTIN, LOW); delay(100); }

  // Instantiate module
  modules[PEAPOD_MODULENUM] = new PeaPodModule();

  // Load Devices
  modules[PEAPOD_MODULENUM]->operator()<HT16K33>(I2CIP::sevenSegmentFQA, true, _i2cip_args_io_default, DebugJsonBreakpoints);
  // modules[PEAPOD_MODULENUM]->operator()<PCA9685>(fqa_pca9685, false, _i2cip_args_io_default, DebugJsonBreakpoints);
  modules[PEAPOD_MODULENUM]->operator()<SHT45>(fqa_sht45, true, _i2cip_args_io_default, DebugJsonBreakpoints);
  modules[PEAPOD_MODULENUM]->operator()<JHD1313>(fqa_lcd, true, _i2cip_args_io_default, DebugJsonBreakpoints);
  modules[PEAPOD_MODULENUM]->operator()<Seesaw>(fqa_rotary, true, _i2cip_args_io_default, DebugJsonBreakpoints);
}

// LOOP GLOBALS
unsigned long last = 0;
unsigned long lastHeartbeat = 0;
uint32_t fps = 0; // Something other than zero
bool revision = false;

// bool temphum = false;
state_sht45_t temphum = {NAN, NAN};
int32_t rotary_zero1 = 0;
int32_t rotary_zero2 = 0;
unsigned long last_lcd = LCD_REFRESH_MAX; unsigned long last_rgb = RGB_REFRESH_MAX; // Fixes first-frame bug
bool do_lcd = true;
float nunchuck_sum = 0.0f;

// PeaPod Stuff
uint8_t peapod_pwm1 = 0; // Pin 15
uint8_t peapod_pwm2 = 0; // Pin 14
bool peapod_toggle = false; // Pin 33
bool peapod_out = false; // Pin 32
bool flag_debounce = false; // Used to debounce for toggles

void loop(void) {
  last = millis();
  #ifdef FSM_STATE_H_
    FSM::Chronos.set(last); // Update chronograph and do event/interval conditionals & callbacks(?)
  #endif

  while(Serial.available() > 0) { // With baud 115200, this should not block
    DebugJson::update(Serial, I2CIP::commandRouter);
  }

  if(millis() - lastHeartbeat >= HEARTBEAT_DELAY) {
    DebugJson::heartbeat(millis(), Serial);
    DebugJson::revision(I2CIP_REVISION, Serial);
    DebugJson::telemetry(millis(), fps, "fps", Serial);
    lastHeartbeat = millis();
  }

  // 0. Prep Args: Onboard 7SEG (Default to Snake Mode)
  i2cip_ht16k33_mode_t seg_mode = SEG_SNAKE;
  i2cip_ht16k33_data_t seg_data = { .h = fps };
  i2cip_args_io_t args_7seg = { .a = nullptr, .s = &seg_data, .b = &seg_mode };

  // 1. MUX & EEPROM: Ping and load devices
  errlev[PEAPOD_MODULENUM] = modules[PEAPOD_MODULENUM]->operator()(); // First-Time EEPROM Self-Registration and Discovery; Ping MUX && EEPROM
  if(errlev[PEAPOD_MODULENUM] == I2CIP_ERR_NONE) {

    // 2. DebugJson Heartbeat & Display FPS
    DebugJson::revision(0, Serial); // sends revision
    seg_mode = SEG_UINT;

    // 3. Prep Args: LCD
    String msg = String(fps) + "Hz I2CIP\n"; // Further append will be on second line
    i2cip_jhd1313_args_t rgb = JHD1313::randomRGBLCD();
    i2cip_args_io_t args_lcd = { .a = nullptr, .s = nullptr, .b = &rgb };
    
    // Prep Args: 7SEG
    // seg_data.f = NAN; // Will produce "NUL.L" on display instead of temperature
    // args_7seg.s = &seg_data.f;

    // SHT45 - Average and print to LCD
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

        DebugJson::telemetry(d->getLastRX(), d->getCache().temperature, "temperature");
        DebugJson::telemetry(d->getLastRX(), d->getCache().humidity, "humidity");
      }

      if(c > 0) {

        th.temperature /= c; th.humidity /= c;

        msg += '[' + String(th.temperature, 1) + "C, " + String(th.humidity, 1) + "%]"; // The double-space might cut off the % but who cares
        seg_data.f = th.temperature;
        seg_mode = SEG_1F;

      } else {
        // LCD Default
        msg += "SHT45 EIO :(";
        args_lcd.s = &msg;
        do_lcd = true;
      }

      // If messages match and neither temperature nor humidity have changed (and are not NaN), skip LCD
      if(!isnan(temphum.temperature) && !isnan(temphum.humidity) && (abs(th.temperature - temphum.temperature) < EPSILON_TEMPERATURE && abs(th.humidity - temphum.humidity) < EPSILON_HUMIDITY) && ((long int)millis() - last_lcd < LCD_REFRESH_MAX)) {
        // MAIN_DEBUG_SERIAL.println(F("[I2CIP | LCD SKIP; RGB ONLY]"));
        do_lcd = false;
      } else {
        temphum.temperature = th.temperature; temphum.humidity = th.humidity;
        args_lcd.s = &msg;
        do_lcd = true;
      }
    } else {
      // LCD Default
      msg += "SHT45 ENOENT :(";
      args_lcd.s = &msg;
      do_lcd = true;
    }

    // Nunchuck
    I2CIP::DeviceGroup* dg_nunchuck = modules[PEAPOD_MODULENUM]->operator[]("NUNCHUCK");
    if(dg_nunchuck != nullptr && dg_nunchuck->getNumDevices() > 0) {
      Nunchuck* d = (Nunchuck*)(dg_nunchuck->getDevice(0)); // Use first device
      if(d != nullptr && d->getInput() != nullptr) {

        i2cip_errorlevel_t errlev_nunchuck = modules[PEAPOD_MODULENUM]->operator()<Nunchuck>(d->getFQA(), true, _i2cip_args_io_default, DebugJsonBreakpoints);

        if(errlev_nunchuck != I2CIP_ERR_NONE) {
          nunchuck_sum += ((float)(d->getCache().x) - 127.f) / 255.f; // += cache.y;

          DebugJson::telemetry(d->getLastRX(), d->getCache().x, "joy_x");
          DebugJson::telemetry(d->getLastRX(), d->getCache().y, "joy_y");
          DebugJson::telemetry(d->getLastRX(), d->getCache().c, "button_c");
          DebugJson::telemetry(d->getLastRX(), d->getCache().z, "button_z");
        } else {
          nunchuck_sum = 0.0f;
        }
      } else {
        nunchuck_sum = 0.0f;
      }
    } else {
      nunchuck_sum = 0.0f;
    }
    
    I2CIP::DeviceGroup* dg_rotary = modules[PEAPOD_MODULENUM]->operator[]("SEESAW");
    if(dg_rotary != nullptr && dg_rotary->getNumDevices() > 0) {
      RotaryEncoder* rotary = (RotaryEncoder*)(dg_rotary->getDevice(0)); // Use first device
      if(rotary != nullptr) {
        i2cip_errorlevel_t errlev_rotary = modules[PEAPOD_MODULENUM]->operator()<Seesaw>(rotary->getFQA(), true, _i2cip_args_io_default, DebugJsonBreakpoints);
        if(errlev_rotary == I2CIP_ERR_NONE) {
          i2cip_rotaryencoder_t cache = rotary->getCache();

          // PeaPod Stuff
          if(cache.button == HIGH) { // Debounce to pulse ONCE on button press
            if(!flag_debounce){
              // Invert and set debounce
              peapod_toggle = !peapod_toggle;
              flag_debounce = true;
            }
            peapod_out = true;
          } else {
            // Reset debounce
            flag_debounce = false;
            peapod_out = false;
          }
          uint32_t angle_ticks = 0;

          // Toggle also selects PWM control target
          if(peapod_toggle) {
            if(cache.button) rotary_zero1 = cache.encoder; // Set zero-point
            angle_ticks = -(cache.encoder - rotary_zero1); // Invert rotation
            peapod_pwm1 = (uint8_t)((float)(Seesaw::_encoderDegrees(angle_ticks)) / 360 * 255); // 0-3840 (256 ticks) -> 0-255
          } else {
            if(cache.button) rotary_zero2 = cache.encoder; // Set zero-point
            angle_ticks = -(cache.encoder - rotary_zero2); // Invert rotation
            peapod_pwm2 = (uint8_t)((float)(Seesaw::_encoderDegrees(angle_ticks)) / 360 * 255); // 0-3840 (256 ticks) -> 0-255
          }
          uint32_t position = Seesaw::_encoderDegrees(angle_ticks, 9999); // 27 * 360 = 9720; Maximum revolutions on 4 digit display

          if(args_lcd.s != nullptr) {
            if(peapod_toggle) {
              *((String*)args_lcd.s) += " B";
            } else {
              *((String*)args_lcd.s) += " R";
            }
          }

          // DebugJson::telemetry(rotary->getLastRX(), cache.encoder, "encoder");
          DebugJson::telemetry(rotary->getLastRX(), position, "position");
          DebugJson::telemetry(rotary->getLastRX(), cache.button, "button");
          // DebugJson::telemetry(rotary->getLastRX(), ((float)peapod_pwm1 / 255), "peapod_pwm1");
          // DebugJson::telemetry(rotary->getLastRX(), ((float)peapod_pwm2 / 255), "peapod_pwm2");
          // DebugJson::telemetry(rotary->getLastRX(), peapod_toggle, "peapod_toggle");
          // DebugJson::telemetry(rotary->getLastRX(), peapod_out, "peapod_out");
          
          // Overwrite 7Seg Args
          seg_mode = SEG_UINT;
          seg_data.h = position + ((unsigned)nunchuck_sum); // Now h is the active member
        }
      }

    }
    //  else {
    //   rotary_zero = 0;
    // }

    if(do_lcd || (long int)millis() - last_rgb > RGB_REFRESH_MAX) {
    // LCD: On-Module SHT45 Status Display
      i2cip_errorlevel_t errlev_lcd = modules[PEAPOD_MODULENUM]->operator()<JHD1313>(fqa_lcd, true, args_lcd, DebugJsonOut);
      if(errlev_lcd == I2CIP_ERR_NONE && fps != 0) {
        if(args_lcd.s != nullptr) last_lcd = millis();
        last_rgb = millis();
      }
    }
  } else {
    seg_mode = SEG_SNAKE;
  }
  // 7SEG: Off-Module (MCU Featherwing/Shield) Multi-Status Display: Rotary, else SHT45, else Snake
  i2cip_errorlevel_t errlev_7seg = modules[PEAPOD_MODULENUM]->operator()<HT16K33>(I2CIP::sevenSegmentFQA, true, args_7seg, DebugJsonOut);

  // PeaPod Stuff
  controlPin<PEAPOD_PIN_TOGGLE>(peapod_toggle);
  controlPin<PEAPOD_PIN_OUT>(peapod_out);

  controlPWM<PEAPOD_PIN_PWM1>(peapod_pwm1);
  controlPWM<PEAPOD_PIN_PWM2>(peapod_pwm2);

  #ifdef CYCLE_DELAY
  delay(CYCLE_DELAY);
  #endif

  // DEBUG PRINT: CYCLE COUNT, FPS, and ERRLEV
  unsigned long delta = millis() - last;
  fps += 1000.f / max(1.f, (float)delta);
  fps /= 2;
}

#endif