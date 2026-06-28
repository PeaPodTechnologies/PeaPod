#include "menu.h"

using namespace PeaPod::Menu;

i2cip_errorlevel_t PeaPod::Menu::errlev_lcd = I2CIP_ERR_NONE;
i2cip_errorlevel_t PeaPod::Menu::errlev_encoder = I2CIP_ERR_NONE;

peapod_menustate_t PeaPod::Menu::_peapod_menustate_default = {
  .menu = PEAPOD_MENU_MAIN,
  .submenu = 0, 
  .module_idx = 0
};

FSM::State<peapod_menustate_t> PeaPod::Menu::menu_state = FSM::State<peapod_menustate_t>(_peapod_menustate_default, "menu");

i2cip_errorlevel_t PeaPod::Menu::initializeDevices(void) {
  errlev_lcd = nomodule.operator()<JHD1313>(fqa_lcd, false, _i2cip_args_io_default, DebugJsonOut);
  I2CIP_ERR_BREAK(errlev_lcd);

  errlev_encoder = nomodule.operator()<Seesaw>(fqa_encoder, false, _i2cip_args_io_default, DebugJsonOut);
  I2CIP_ERR_BREAK(errlev_encoder);

  if(errlev_lcd == I2CIP_ERR_NONE) {
    String init = errlev_encoder ? PEAPOD_MENU_STR_ENCODERFAIL : PEAPOD_MENU_STR_INITIALIZING;
    i2cip_args_io_t args = _i2cip_args_io_default;
    args.g = false;
    args.s = &init;
    i2cip_jhd1313_args_t rgb = JHD1313::randomRGBLCD();
    i2cip_args_io_t args_lcd = { .a = nullptr, .s = nullptr, .b = &rgb };
    errlev_lcd = nomodule.operator()<JHD1313>(fqa_lcd, true, args, DebugJsonOut);

    menu_state.addCallback(writeLCDMenu);
  }
  return errlev_lcd;
}

int32_t encoder_last = 0;
bool toggle = false;

i2cip_errorlevel_t PeaPod::Menu::update(void) {
  errlev_encoder = nomodule.operator()<Seesaw>(fqa_encoder, true, _i2cip_args_io_default, DebugJsonOut);
  if(errlev_encoder == I2CIP_ERR_NONE) {
    // Read encoder state and update menu
    Device** dptr = devicetree[fqa_encoder];
    if(dptr && *dptr) {
      RotaryEncoder* encoder = (RotaryEncoder*)(*dptr);
      int32_t encoder_pos = encoder->getCache().encoder;
      if(encoder_pos != encoder_last) {
        onEncoderChange(encoder_last > encoder_pos);
      }
      if(encoder->getCache().button == PIN_ON && !toggle) {
        onEncoderPress();
        toggle = true;
      }
      if(encoder->getCache().button == PIN_OFF) {
        toggle = false;
      }
      encoder_last = encoder_pos;
    } else {
      // TODO: Ping and add
    }
  } else {
    String init = PEAPOD_MENU_STR_ENCODERFAIL;
    i2cip_args_io_t args = _i2cip_args_io_default;
    args.g = false;
    args.s = &init;
    i2cip_jhd1313_args_t rgb = JHD1313::randomRGBLCD();
    i2cip_args_io_t args_lcd = { .a = nullptr, .s = nullptr, .b = &rgb };
    errlev_lcd = nomodule.operator()<JHD1313>(fqa_lcd, true, args, DebugJsonOut);
  }
  return errlev_lcd;
}

void PeaPod::Menu::onEncoderChange(bool inc)  {
  peapod_menustate_t menu = menu_state.get();

  PeaPodModule* currentModule = (PeaPodModule*)modules[menu.module_idx];
  uint8_t moduleNumFlags = currentModule == nullptr ? 0 : currentModule->getNumFlags();
  uint8_t moduleNumVariables = currentModule == nullptr ? 0 : currentModule->getNumVariables();
  FSM::Variable* variable = currentModule == nullptr ? nullptr : currentModule->getVariableByIndex(menu.submenu - 1);

  int newSubmenu = (int)menu.submenu + (inc ? 1 : -1);
  switch(menu.menu) {
    case PEAPOD_MENU_MAIN:
      if(newSubmenu < 0) newSubmenu = PEAPOD_MENU_SUBMENUS_MAIN - (abs(newSubmenu) % PEAPOD_MENU_SUBMENUS_MAIN); 
      newSubmenu %= PEAPOD_MENU_SUBMENUS_MAIN; 
      break;
    case PEAPOD_MENU_MODULE:
      if(newSubmenu < 0) newSubmenu = PEAPOD_MENU_SUBMENUS_MODULE - (abs(newSubmenu) % PEAPOD_MENU_SUBMENUS_MODULE);
      newSubmenu %= PEAPOD_MENU_SUBMENUS_MODULE; // 8 Modules + Back 
      break;
    case PEAPOD_MENU_DEVICE:
      if(newSubmenu < 0) newSubmenu = devicetree.size() - (abs(newSubmenu) % devicetree.size());
      newSubmenu %= (devicetree.size() + 1); // All Devices + Back
      break;
    case PEAPOD_MENU_FLAG:
      if(newSubmenu < 0) newSubmenu = moduleNumFlags - (abs(newSubmenu) % moduleNumFlags);
      newSubmenu %= (moduleNumFlags + 1); // All Flags for the selected module + Back
      break;
    case PEAPOD_MENU_VARIABLE:
      if(newSubmenu < 0) newSubmenu = moduleNumVariables - (abs(newSubmenu) % moduleNumVariables);
      newSubmenu %= (moduleNumVariables + 1); // All Variables for the selected module + Back
      break;
    case PEAPOD_MENU_SET:
      if(variable != nullptr) {
        double val = variable->get();
        if(inc) {
          val += 1;
        } else {
          if(val < 0) val += 1;
          else val -= 1;
        }
        variable->set(val);
      }
  }

  if(menu.menu != PEAPOD_MENU_SET) menu.submenu = newSubmenu;

  menu_state.set(menu);
}

void PeaPod::Menu::writeLCDMenu(bool _, const peapod_menustate_t& menu) {
  FSM::Flag* flag;
  FSM::Variable* variable;
  String line1 = "", line2 = ""; // LCD has 2 lines of 16 characters each
  switch(menu.menu) {
    case PEAPOD_MENU_MAIN:
      line1 = "PeaPod";
      switch(menu.submenu) {
        case 0:
          line2 = "Summary";
          break;
        case 1:
          line2 = "Select Module";
          break;
        case 2:
          line2 = "Select Device";
          break;
        case 3:
          line2 = "Select Flag";
          break;
        case 4:
          line2 = "Select Variable";
          break;
      }
      break;
    case PEAPOD_MENU_MODULE:
      line1 = menu.submenu == 0 ? "Modules" : String("Module ") + String(menu.submenu - 1);
      if(menu.submenu == 0) {
        line2 = "BACK";
        break;
      }
      if(menu.submenu - 1 < 8) {
        // Display module info on line 2
        if(modules[menu.submenu - 1]) {
          line2 = "FOUND 0x" + String(errlev[menu.submenu - 1], HEX);
        } else {
          line2 = "ENOENT";
        }
      }
      break;
    case PEAPOD_MENU_DEVICE:
      line1 = menu.submenu == 0 ? "Devices" : String("Device ") + String(menu.submenu - 1);
      if(menu.submenu == 0) {
        line2 = "BACK";
        break;
      }
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
    flag = ((PeaPodModule*)modules[menu.module_idx])->getFlagByIndex(menu.submenu - 1);
    line1 = menu.submenu == 0 ? "Flags" : String("F ") + (flag == nullptr ? "ENOENT" : flag->getKey());
    if(menu.submenu == 0) {
        line2 = "BACK";
        break;
      }
      if(flag == nullptr) {
        line2 = "ENOENT";
        break;
      }
      line2 = flag->get() ? "ON" : "OFF";
      break;
    case PEAPOD_MENU_VARIABLE:
      variable = ((PeaPodModule*)modules[menu.module_idx])->getVariableByIndex(menu.submenu - 1);
      line1 = menu.submenu == 0 ? "Variables" : String("V ") + (variable == nullptr ? "ENOENT" : variable->getKey());
      if(menu.submenu == 0) {
        line2 = "BACK";
        break;
      }
      if(variable == nullptr) {
        line2 = "ENOENT";
        break;
      }
      line2 = String(variable->get(), 3); // 3 decimal places
      break;
    case PEAPOD_MENU_SET:
      variable = ((PeaPodModule*)modules[menu.module_idx])->getVariableByIndex(menu.submenu - 1);
      line1 = menu.submenu == 0 ? "Variables" : String("S ") + (variable == nullptr ? "ENOENT" : variable->getKey());
      if(menu.submenu == 0) {
        line2 = "BACK";
        break;
      }
      if(variable == nullptr) {
        line2 = "ENOENT";
        break;
      }
      line2 = String(variable->get(), 3); // 3 decimal places
      break;
  }

  String display = line1 + "\n" + line2;

  i2cip_args_io_t args = _i2cip_args_io_default;
  args.g = false;
  args.s = &display;
  args.b = nullptr;

  errlev_lcd = nomodule.operator()<JHD1313>(fqa_lcd, true, args, DebugJsonOut);
}

void PeaPod::Menu::onEncoderPress(void) {
  peapod_menustate_t menu = menu_state.get();

  FSM::Flag* flag;

  switch(menu.menu) {
    case PEAPOD_MENU_MAIN:
      if(menu.submenu == 0) {
        DebugJson::revision(0, PEAPOD_SERIAL); // FOR NOW
      } else {
        switch(menu.submenu) {
          case 1:
            menu.menu = PEAPOD_MENU_MODULE;
            break;
          case 2:
            menu.menu = PEAPOD_MENU_DEVICE;
            break;
          case 3:
            menu.menu = PEAPOD_MENU_FLAG;
            break;
          case 4:
            menu.menu = PEAPOD_MENU_VARIABLE;
            break;
          case 5:
            menu.menu = PEAPOD_MENU_SET;
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
        menu.menu = PEAPOD_MENU_SET; // Go to variable set menu
      }
      break;
    case PEAPOD_MENU_SET:
      menu.menu = PEAPOD_MENU_VARIABLE; // Go back to variable view
  }

  menu_state.set(menu);
}