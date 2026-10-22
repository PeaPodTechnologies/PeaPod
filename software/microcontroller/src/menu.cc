#include "menu.h"

using namespace PeaPod::Menu;

uint32_t PeaPod::Menu::code = 0;
i2cip_errorlevel_t PeaPod::Menu::errlev_lcd = I2CIP_ERR_NONE;
i2cip_errorlevel_t PeaPod::Menu::errlev_encoder = I2CIP_ERR_NONE;
i2cip_errorlevel_t PeaPod::Menu::errlev_sevenseg = I2CIP_ERR_NONE;

peapod_menustate_t PeaPod::Menu::_peapod_menustate_default = {
  .menu = PEAPOD_MENU_MAIN,
  .submenu = 0, 
  .module_idx = 0
};

#ifndef PEAPOD_SEVENSEG_TEMPERATURE
#ifndef PEAPOD_SEVENSEG_HUMIDITY
#ifndef PEAPOD_SEVENSEG_CO2
#ifndef PEAPOD_SEVENSEG_ADC
void writeSevenSegmentErrorCode() {
  // Clear the last 3 bits and set based on error levels
  code = code & ~0b111 |
    (errlev_sevenseg > I2CIP_ERR_NONE ? 0b1 : 0) | 
    (errlev_lcd > I2CIP_ERR_NONE ? 0b10 : 0) | 
    (errlev_encoder > I2CIP_ERR_NONE ? 0b100 : 0);
  writeSevenSegmentCode();
}
#endif
#endif
#endif
#endif

FSM::State<peapod_menustate_t> PeaPod::Menu::menu_state = FSM::State<peapod_menustate_t>(_peapod_menustate_default, "menu");

i2cip_errorlevel_t PeaPod::Menu::initialize(void) {
  errlev_lcd = notamodule.operator()<MCP23008>(fqa_lcd, false, _i2cip_args_io_default, DebugJsonOut);
  I2CIP_ERR_BREAK(errlev_lcd);

  errlev_encoder = notamodule.operator()<Seesaw>(fqa_encoder, false, _i2cip_args_io_default, DebugJsonOut);
  I2CIP_ERR_BREAK(errlev_encoder);

  if(errlev_lcd == I2CIP_ERR_NONE) {
    String init = errlev_encoder ? PEAPOD_MENU_STR_ENCODERFAIL : PEAPOD_MENU_STR_INITIALIZING;
    i2cip_args_io_t args = _i2cip_args_io_default;
    args.g = false;
    args.s = &init;
    // i2cip_jhd1313_args_t rgb = JHD1313::randomRGBLCD();
    // i2cip_args_io_t args_lcd = { .a = nullptr, .s = nullptr, .b = &rgb };
    // errlev_lcd = nomodule.operator()<MCP23008>(fqa_lcd, true, _i2cip_args_io_default, DebugJsonOut);

    MCP23008** dptr = (MCP23008**)devicetree.operator[](fqa_lcd);
    if(dptr && *dptr) {
      errlev_lcd = LCD(*dptr).set(init, LCD_ARGS_NONE);
    }
  }

  menu_state.addCallback(writeLCDMenu);
  
  return errlev_lcd;
}

int32_t encoder_last = 0;
bool toggle = false;
bool toggle_hold = false;
FSM::fsm_timestamp_t rising = 0;

bool initialized = false;

i2cip_errorlevel_t PeaPod::Menu::update(void) {

  if (!initialized) {
    writeLCDMenu(false, menu_state.get());
    initialized = true;
  }

  errlev_encoder = notamodule.operator()<Seesaw>(fqa_encoder, true, _i2cip_args_io_default, DebugJsonOut);
  if (errlev_encoder == I2CIP_ERR_NONE) {
    // Read encoder state and update menu
    Device** dptr = devicetree[fqa_encoder];
    if (dptr && *dptr) {
      RotaryEncoder* encoder = (RotaryEncoder*)(*dptr);
      errlev_encoder = notamodule.operator()<Seesaw>(fqa_encoder, true, _i2cip_args_io_default, DebugJsonOut);
      int32_t encoder_pos = encoder->getCache().encoder;
      i2cip_state_pin_t button = encoder->getCache().button;

      code = code & ~0x10 | (button == PIN_ON ? 0x10 : 0); // Show encoder button state on 7-segment display position 2
      if(encoder_pos > encoder_last) {
        code = code & ~0x1100 | ((1 << 8) & 0x1100); // Show encoder advance on 7-segment display position 3
      } else if(encoder_pos < encoder_last) {
        code = code & ~0x1100 | ((1 << 12) & 0x1100); // Show encoder reverse on 7-segment display position 4
      } else {
        code = code & ~0x1100; // Clear encoder advance/reverse indicators on 7-segment display positions 3 and 4
      }

      #ifndef PEAPOD_SEVENSEG_TEMPERATURE
      #ifndef PEAPOD_SEVENSEG_HUMIDITY
      #ifndef PEAPOD_SEVENSEG_CO2
      #ifndef PEAPOD_SEVENSEG_ADC
        writeSevenSegmentErrorCode();
      #endif
      #endif
      #endif
      #endif

      if (encoder_pos != encoder_last) {
        onEncoderChange(encoder_last > encoder_pos);
      }

      unsigned long now = millis();

      // Rising edge
      if (button == PIN_ON && !toggle) {
        rising = now;
        toggle = true;
        toggle_hold = false;
      }
      
      if (toggle && !toggle_hold && (now - rising >= PEAPOD_ENCODER_HOLD_THRESHOLD)) {
        onEncoderHold();
        toggle_hold = true;
      }

      // Falling edge
      if (button == PIN_OFF && toggle) {
        if (!toggle_hold && (now - rising < PEAPOD_ENCODER_HOLD_THRESHOLD)) {
          onEncoderPress();
        }
        toggle = false;
        toggle_hold = false;
      }

      errlev_encoder = notamodule.operator()<Seesaw>(fqa_encoder, true, _i2cip_args_io_default, DebugJsonOut); // Fixes Issue #139
      encoder_last = encoder->getCache().encoder;
    } else {
      // TODO: Ping and add
    }
  } else {
    String fail = PEAPOD_MENU_STR_ENCODERFAIL;
    // i2cip_args_io_t args = _i2cip_args_io_default;
    // args.g = false;
    // args.s = &init;
    // i2cip_jhd1313_args_t rgb = JHD1313::randomRGBLCD();
    // i2cip_args_io_t args_lcd = { .a = nullptr, .s = nullptr, .b = &rgb };
    // errlev_lcd = nomodule.operator()<JHD1313>(fqa_lcd, true, args, DebugJsonOut);
    MCP23008** dptr = (MCP23008**)devicetree.operator[](fqa_lcd);
    if(dptr && *dptr) {
      MCP23008* mcp = *dptr;
      errlev_lcd = LCD(mcp).set(fail, LCD_ARGS_NONE);
    } else errlev_lcd = I2CIP_ERR_SOFT;
  }

  #ifndef PEAPOD_SEVENSEG_TEMPERATURE
  #ifndef PEAPOD_SEVENSEG_HUMIDITY
  #ifndef PEAPOD_SEVENSEG_CO2
  #ifndef PEAPOD_SEVENSEG_ADC
    writeSevenSegmentCode();
  #endif
  #endif
  #endif
  #endif

  return errlev_lcd;
}

void PeaPod::Menu::onEncoderChange(bool inc)  {
  peapod_menustate_t menu = menu_state.get();

  unsigned numDevices = devicetree.size();
  PeaPodModule* currentModule = (PeaPodModule*)modules[menu.module_idx];
  // Serial.print("-> Encoder change (");
  // if(inc) Serial.print("+");
  // else Serial.print("-");
  // Serial.println(")");
  // Serial.println("-> Current module index: " + String(menu.module_idx) + String(currentModule == nullptr ? " (null)" : " (valid)"));

  uint8_t moduleNumFlags = currentModule == nullptr ? 0 : currentModule->getNumFlags();
  uint8_t moduleNumVariables = currentModule == nullptr ? 0 : currentModule->getNumVariables();
  uint8_t moduleNumIntervals = currentModule == nullptr ? 0 : currentModule->getNumIntervals();
  uint8_t moduleNumEvents = currentModule == nullptr ? 0 : currentModule->getNumEvents();

  // Serial.println("-> Current module number of flags: " + String(moduleNumFlags));
  // Serial.println("-> Current module number of variables: " + String(moduleNumVariables));
  // Serial.println("-> Current module number of intervals: " + String(moduleNumIntervals));
  // Serial.println("-> Current module number of events: " + String(moduleNumEvents));
  FSM::Variable* variable = currentModule == nullptr ? nullptr : currentModule->getVariableByIndex(menu.submenu - 1);
  FSM::IntervalCallback* interval = currentModule == nullptr ? nullptr : currentModule->getIntervalByIndex(menu.submenu - 1);
  FSM::ChronoEvent* event = currentModule == nullptr ? nullptr : currentModule->getEventByIndex(menu.submenu - 1);

  int newSubmenu = (int)menu.submenu + (inc ? 1 : -1);
  switch(menu.menu) {
    case PEAPOD_MENU_MAIN:
      if(newSubmenu < 0) newSubmenu = PEAPOD_MENU_SUBMENUS_MAIN - 1; 
      if(newSubmenu >= PEAPOD_MENU_SUBMENUS_MAIN) newSubmenu = 0;
      break;
    case PEAPOD_MENU_MODULE:
      if(newSubmenu < 0) newSubmenu = PEAPOD_MENU_SUBMENUS_MODULE - 1;
      if(newSubmenu >= PEAPOD_MENU_SUBMENUS_MODULE) newSubmenu = 0;
      break;
    case PEAPOD_MENU_DEVICE:
      if(newSubmenu < 0) newSubmenu = numDevices > 0 ? numDevices - 1 : 0;
      if(newSubmenu > numDevices) newSubmenu = 0;
      break;
    case PEAPOD_MENU_FLAG:
      if(newSubmenu < 0) newSubmenu = moduleNumFlags > 0 ? moduleNumFlags - 1 : 0;
      if(newSubmenu > moduleNumFlags) newSubmenu = 0;
      break;
    case PEAPOD_MENU_VARIABLE:
      if(menu.setting) {
        if(variable != nullptr) {
          double val = variable->get();
          if(inc) {
            val += 1;
          } else {
            if(val < 0) val = 0;
            else val -= 1;
          }
          variable->set(val);
        }
      } else {
        if(newSubmenu < 0) newSubmenu = moduleNumVariables > 0 ? moduleNumVariables - 1 : 0;
        if(newSubmenu > moduleNumVariables) newSubmenu = 0;
      }
      break;
    case PEAPOD_MENU_INTERVAL:
      if(menu.setting) {
        if(interval != nullptr) {
          FSM::fsm_timestamp_t val = interval->getDelta();
          if(inc) {
            val += 1;
          } else {
            if(val <= 0) val = 0;
            else val -= 1;
          }
          interval->setDelta(val);
        }
      } else {
        if(newSubmenu < 0) newSubmenu = moduleNumIntervals > 0 ? moduleNumIntervals - 1 : 0;
        if(newSubmenu > moduleNumIntervals) newSubmenu = 0;
      }
      break;
    case PEAPOD_MENU_EVENT:
      if(menu.setting) {
        if(event != nullptr) {
          FSM::fsm_timestamp_t val = event->getTimestamp();
          if(inc) {
            val += 1;
          } else {
            if(val <= 0) val = 0;
            else val -= 1;
          }
          event->setTimestamp(val);
        }
      } else {
        if(newSubmenu < 0) newSubmenu = moduleNumEvents > 0 ? moduleNumEvents - 1 : 0;
        newSubmenu %= (moduleNumEvents + 1); // All Events for the selected module + Back
      }
      break;
  }

  if(!menu.setting) menu.submenu = newSubmenu;

  menu_state.set(menu);
}

void PeaPod::Menu::writeLCDMenu(bool _, const peapod_menustate_t& menu) {
  FSM::Flag* flag;
  FSM::Variable* variable;
  FSM::IntervalCallback* interval;
  FSM::ChronoEvent* event;
  String line1 = "", line2 = ""; // LCD has 2 lines of 16 characters each
  switch(menu.menu) {
    case PEAPOD_MENU_MAIN:
      line1 = "PeaPod";
      switch(menu.submenu) {
        case PEAPOD_MENU_MAIN:
          line2 = "Summary";
          break;
        case PEAPOD_MENU_MODULE:
          line2 = "Select Module";
          break;
        case PEAPOD_MENU_DEVICE:
          line2 = "Select Device";
          break;
        case PEAPOD_MENU_FLAG:
          line2 = "Select Flag";
          break;
        case PEAPOD_MENU_VARIABLE:
          line2 = "Select Variable";
          break;
        case PEAPOD_MENU_INTERVAL:
          line2 = "Select Interval";
          break;
        case PEAPOD_MENU_EVENT:
          line2 = "Select Event";
          break;
      }
      break;
    case PEAPOD_MENU_MODULE:
      if(menu.submenu == 0) {
        line1 = "Modules";
        line2 = "BACK";
        break;
      }
      line1 = String("Module ") + String(menu.submenu - 1);
      if(menu.submenu - 1 < 8) {
        // Display module info on line 2
        if(modules[menu.submenu - 1] != nullptr) {
          line2 = "FOUND 0x" + String(errlev[menu.submenu - 1], HEX);
        } else {
          line2 = "ENOENT";
        }
      }
      break;
    case PEAPOD_MENU_DEVICE:
      if(menu.submenu == 0) {
        line1 = "Devices";
        line2 = "BACK";
        break;
      }
      line1 = String("Device ") + String(menu.submenu - 1);
      if(menu.submenu - 1 < devicetree.size()) {
        // Display device info on line 2
        Device** dptr = devicetree.getByIndex(menu.submenu - 1);
        if(!dptr || !(*dptr)) {
          line2 = "ENOENT";
          break;
        }
        i2cip_fqa_t fqa = (*dptr)->getFQA();
        line2 = String((*dptr)->getID()) + " " + String(I2CIP_FQA_SEG_MODULE(fqa)) + ":" + String(I2CIP_FQA_SEG_MUXBUS(fqa)) + ":" + String(I2CIP_FQA_SEG_DEVADR(fqa), HEX);
      } else {
        line2 = "ENOENT";
      }
      break;
    case PEAPOD_MENU_FLAG:
      if(menu.submenu == 0) {
        line1 = "Flags";
        line2 = "BACK";
        break;
      }
      flag = ((PeaPodModule*)modules[menu.module_idx])->getFlagByIndex(menu.submenu - 1);
      line1 = String("F ") + (flag == nullptr ? "ENOENT" : flag->getKey());
      if(flag == nullptr) {
        line2 = "ENOENT";
        break;
      }
      line2 = flag->get() ? "ON" : "OFF";
      break;
    case PEAPOD_MENU_VARIABLE:
      if(menu.submenu == 0) {
        line1 = "Variables";
        line2 = "BACK";
        break;
      }
      variable = ((PeaPodModule*)modules[menu.module_idx])->getVariableByIndex(menu.submenu - 1);
      if(menu.setting) {
        line1 = String("V? ") + (variable == nullptr ? "ENOENT" : variable->getKey());
        if(variable == nullptr) {
          line2 = "ENOENT";
          break;
        }
        line2 = String(variable->get(), 3); // 3 decimal places
      } else {
        line1 = String("V ") + (variable == nullptr ? "ENOENT" : variable->getKey());
        
        if(variable == nullptr) {
          line2 = "ENOENT";
          break;
        }
        line2 = String(variable->get(), 3); // 3 decimal places
      }
      break;
    case PEAPOD_MENU_INTERVAL:
      if(menu.submenu == 0) {
        line1 = "Intervals";
        line2 = "BACK";
        break;
      }
      interval = ((PeaPodModule*)modules[menu.module_idx])->getIntervalByIndex(menu.submenu - 1);
      if(menu.setting) {
        line1 = String("I? ") + (interval == nullptr ? "ENOENT" : interval->getKey());
        if(interval == nullptr) {
          line2 = "ENOENT";
          break;
        }
        line2 = FSM::timestampToString(interval->getDelta());
      } else {
        line1 = String("I ") + (interval == nullptr ? "ENOENT" : interval->getKey());
        if(interval == nullptr) {
          line2 = "ENOENT";
          break;
        }
        line2 = FSM::timestampToString(interval->getDelta());
      }
      break;
    case PEAPOD_MENU_EVENT:
      if(menu.submenu == 0) {
        line1 = "Events";
        line2 = "BACK";
        break;
      }
      event = ((PeaPodModule*)modules[menu.module_idx])->getEventByIndex(menu.submenu - 1);
      if(menu.setting) {
        line1 = String("E? ") + (event == nullptr ? "ENOENT" : event->getKey());
        if(event == nullptr) {
          line2 = "ENOENT";
          break;
        }
        // line2 = String(event->get(), 3); // 3 decimal places
        line2 = FSM::timestampToString(event->getTimestamp());
      } else {
        line1 = String("E ") + (event == nullptr ? "ENOENT" : event->getKey());
        if(event == nullptr) {
          line2 = "ENOENT";
          break;
        }
        // line2 = String(event->get(), 3); // 3 decimal places
        line2 = FSM::timestampToString(event->getTimestamp());
      }
      break;
  }

  String display = line1 + "\n" + line2 + "\n\nCurrent Module: " + String(menu.module_idx);

  i2cip_args_io_t args = _i2cip_args_io_default;
  args.g = false;
  args.s = &display;
  args.b = nullptr;

  // errlev_lcd = nomodule.operator()<JHD1313>(fqa_lcd, true, args, DebugJsonOut);
  // if(lcd != nullptr) errlev_lcd = lcd->set(display, LCD_ARGS_NONE);
  // else errlev_lcd = I2CIP_ERR_SOFT;
  MCP23008** dptr = (MCP23008**)devicetree.operator[](fqa_lcd);
  if(dptr && *dptr) {
    errlev_lcd = LCD(*dptr).set(display, LCD_ARGS_NONE);
  } else errlev_lcd = I2CIP_ERR_SOFT;
}

void PeaPod::Menu::onEncoderPress(void) {
  peapod_menustate_t menu = menu_state.get();

  FSM::Flag* flag;

  switch(menu.menu) {
    case PEAPOD_MENU_MAIN:
      if(menu.submenu == PEAPOD_MENU_MAIN) {
        DebugJson::revision(0, PEAPOD_SERIAL); // FOR NOW
      } else {
        switch(menu.submenu) {
          case PEAPOD_MENU_MODULE:
            menu.menu = PEAPOD_MENU_MODULE;
            break;
          case PEAPOD_MENU_DEVICE:
            menu.menu = PEAPOD_MENU_DEVICE;
            break;
          case PEAPOD_MENU_FLAG:
            menu.menu = PEAPOD_MENU_FLAG;
            break;
          case PEAPOD_MENU_VARIABLE:
            menu.menu = PEAPOD_MENU_VARIABLE;
            break;
          case PEAPOD_MENU_INTERVAL:
            menu.menu = PEAPOD_MENU_INTERVAL;
            break;
          case PEAPOD_MENU_EVENT:
            menu.menu = PEAPOD_MENU_EVENT;
            break;
          default:
            break;
        }
        menu.submenu = 0;
      }
      break;
    case PEAPOD_MENU_MODULE:
      if(menu.submenu == 0) {
        menu.menu = PEAPOD_MENU_MAIN; // Back to main menu
        menu.submenu = 0;
      } else {
        menu.module_idx = menu.submenu - 1; // Set selected module index for use in the flag/variable menu
        menu.submenu = 0;
        break;
      }
      break;
    case PEAPOD_MENU_DEVICE:
      if(menu.submenu == 0) {
        menu.menu = PEAPOD_MENU_MAIN; // Back to main menu
        menu.submenu = 0;
      } else {
        // NOP for now - could implement device-specific actions here
        menu.submenu = 0;
      }
      break;
    case PEAPOD_MENU_FLAG:
      if(menu.submenu == 0) {
        menu.menu = PEAPOD_MENU_MAIN; // Back to main menu
        menu.submenu = 0;
        break;
      }
      flag = ((PeaPodModule*)modules[menu.module_idx])->getFlagByIndex(menu.submenu - 1);
      if(flag == nullptr) {
        break;
      }
      flag->set(!flag->get()); // Toggle the selected flag
      break;
    case PEAPOD_MENU_VARIABLE:
      if(menu.submenu == 0) {
        menu.menu = PEAPOD_MENU_MAIN; // Back to main menu
        menu.submenu = 0;
      } else {
        // menu.menu = PEAPOD_MENU_SET; // Go to variable set menu
        menu.setting = !menu.setting;
      }
      break;
    case PEAPOD_MENU_INTERVAL:
      if(menu.submenu == 0) {
        menu.menu = PEAPOD_MENU_MAIN; // Back to main menu
        menu.submenu = 0;
      } else {
        menu.setting = !menu.setting;
      }
      break;
    case PEAPOD_MENU_EVENT:
      if(menu.submenu == 0) {
        menu.menu = PEAPOD_MENU_MAIN; // Back to main menu
        menu.submenu = 0;
      } else {
        menu.setting = !menu.setting;
      }
      break;
  }

  menu_state.set(menu);
}

void PeaPod::Menu::onEncoderHold(void) {
  // peapod_menustate_t menu = menu_state.get();

  // switch(menu.menu) {
  //   case PEAPOD_MENU_INTERVAL:
  //     // Handle interval toggle logic here if needed

  //     break;
  //   default:
  //     break;
  // }
}

void PeaPod::Menu::writeSevenSegmentCode(void) {
  code = code & ~0b111 |
    (errlev_sevenseg > I2CIP_ERR_NONE ? 0b1 : 0) | 
    (errlev_lcd > I2CIP_ERR_NONE ? 0b10 : 0) | 
    (errlev_encoder > I2CIP_ERR_NONE ? 0b100 : 0);
  i2cip_ht16k33_data_t data = { .h = (int32_t)PeaPod::Menu::code };
  i2cip_ht16k33_mode_t mode = SEG_HEX16;
  i2cip_args_io_t args_sevenseg = { .g = false, .a = nullptr, .s = &data, .b = &mode };
  errlev_sevenseg = PeaPod::notamodule.operator()<HT16K33>(fqa_sevenseg, true, args_sevenseg, DebugJsonOut);
}