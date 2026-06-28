#ifndef PEAPOD_MENU_H
#define PEAPOD_MENU_H

#include <Arduino.h>

#include <peapod.h>

#define PEAPOD_MENU_SUBMENUS_MAIN 5 // Summary, Select Module & Device & Flag & Variable = 5
#define PEAPOD_MENU_SUBMENUS_MODULE 9 // 8 Modules + Back
#define PEAPOD_MENU_STR_INITIALIZING "PeaPod\nInitializing..."
#define PEAPOD_MENU_STR_ENCODERFAIL "PeaPod\nENCODER FAIL"

using namespace I2CIP;

namespace PeaPod {
  namespace Menu {
    typedef enum PEAPOD_MENU_T {
      PEAPOD_MENU_MAIN = 0,
      PEAPOD_MENU_MODULE = 1,
      PEAPOD_MENU_DEVICE = 2,
      PEAPOD_MENU_FLAG = 3,
      PEAPOD_MENU_VARIABLE = 4,
      PEAPOD_MENU_SET = 5,
    } PEAPOD_MENU_T;

    extern struct peapod_menustate_s {
      PEAPOD_MENU_T menu; // Top-level functionality selection
      uint8_t submenu;  // Ordinal of the selected item in the current menu, 0 is always "BACK"
      uint8_t module_idx;
    } _peapod_menustate_default;

    typedef struct peapod_menustate_s peapod_menustate_t;

    extern FSM::State<peapod_menustate_t> menu_state;

    const i2cip_fqa_t fqa_lcd = createFQA(PEAPOD_WIRENUM, I2CIP_MUX_NUM_FAKE, I2CIP_MUX_BUS_FAKE, I2CIP_JHD1313_ADDRESS);
    const i2cip_fqa_t fqa_encoder = createFQA(PEAPOD_WIRENUM, I2CIP_MUX_NUM_FAKE, I2CIP_MUX_BUS_FAKE, I2CIP_SEESAW_ADDRESS);

    extern i2cip_errorlevel_t errlev_lcd;
    extern i2cip_errorlevel_t errlev_encoder;

    void onEncoderChange(bool inc);
    void writeLCDMenu(bool _, const peapod_menustate_t& menu);
    void onEncoderPress(void);

    i2cip_errorlevel_t initializeDevices(void);

    // Reads encoder, triggers state update, LCD write callback
    i2cip_errorlevel_t update(void);
  }
}

template <> FSM::State<PeaPod::Menu::peapod_menustate_t>::~State() = default;
template <> FSM::ConditionalCallback<PeaPod::Menu::peapod_menustate_t>* FSM::State<PeaPod::Menu::peapod_menustate_t>::addCallback(typename FSM::ConditionalCallback<PeaPod::Menu::peapod_menustate_t>::cb_compval_t cb) { return addConditional(new FSM::ConditionalCallback<PeaPod::Menu::peapod_menustate_t>(this->key, CMP_NOP, PeaPod::Menu::_peapod_menustate_default, cb)); }
template <> bool FSM::ConditionalCallback<PeaPod::Menu::peapod_menustate_t>::compare(comparators_t cmp, const PeaPod::Menu::peapod_menustate_t& val, const PeaPod::Menu::peapod_menustate_t& ref) { return true; } // Every comparison is unequal, since we only use unconditional callbacks

#endif