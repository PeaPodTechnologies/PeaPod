#ifndef PEAPOD_MENU_H
#define PEAPOD_MENU_H

#include <Arduino.h>

#include <peapod.h>

#define PEAPOD_MENU_SUBMENUS_MAIN 7 // Summary, Select Module & Device & Flag & Variable & Interval & Event = 7
#define PEAPOD_MENU_SUBMENUS_MODULE 9 // 8 Modules + Back
#define PEAPOD_MENU_STR_INITIALIZING "PeaPod\nInitializing..."
#define PEAPOD_MENU_STR_ENCODERFAIL "PeaPod\nENCODER FAIL"
#define PEAPOD_ENCODER_HOLD_THRESHOLD 3000

using namespace I2CIP;

namespace PeaPod {
  namespace Menu {
    typedef enum PEAPOD_MENU_T {
      PEAPOD_MENU_MAIN = 0,
      PEAPOD_MENU_MODULE = 1,
      PEAPOD_MENU_DEVICE = 2,
      PEAPOD_MENU_FLAG = 3,
      PEAPOD_MENU_VARIABLE = 4,
      PEAPOD_MENU_INTERVAL = 5,
      PEAPOD_MENU_EVENT = 6,
    } PEAPOD_MENU_T;

    extern struct peapod_menustate_s {
      PEAPOD_MENU_T menu; // Top-level functionality selection
      uint8_t submenu;  // Ordinal of the selected item in the current menu, 0 is always "BACK"
      uint8_t module_idx;
      bool setting; // Indicates if the menu is currently in a setting state
    } _peapod_menustate_default;

    extern uint32_t code;

    typedef struct peapod_menustate_s peapod_menustate_t;

    extern FSM::State<peapod_menustate_t> menu_state;

    const i2cip_fqa_t fqa_lcd = createFQA(PEAPOD_WIRENUM, I2CIP_MUX_NUM_FAKE, I2CIP_MUX_BUS_FAKE, I2CIP_MCP23008_ADDRESS);
    const i2cip_fqa_t fqa_encoder = createFQA(PEAPOD_WIRENUM, I2CIP_MUX_NUM_FAKE, I2CIP_MUX_BUS_FAKE, I2CIP_SEESAW_ADDRESS);

    extern i2cip_errorlevel_t errlev_lcd;
    extern i2cip_errorlevel_t errlev_encoder;
    extern i2cip_errorlevel_t errlev_sevenseg;

    void onEncoderChange(bool inc);
    void writeLCDMenu(bool _, const peapod_menustate_t& menu);
    void writeSevenSegmentCode(void);
    void onEncoderPress(void);
    void onEncoderHold(void);

    i2cip_errorlevel_t initialize(void);

    // Reads encoder, triggers state update, LCD write callback
    i2cip_errorlevel_t update(void);
  }
}

template <> inline FSM::State<PeaPod::Menu::peapod_menustate_t>::~State() = default;
template <> inline FSM::ConditionalCallback<PeaPod::Menu::peapod_menustate_t>* FSM::State<PeaPod::Menu::peapod_menustate_t>::addCallback(typename FSM::ConditionalCallback<PeaPod::Menu::peapod_menustate_t>::cb_compval_t cb) { return addConditional(new FSM::ConditionalCallback<PeaPod::Menu::peapod_menustate_t>(this->key, CMP_NOP, PeaPod::Menu::_peapod_menustate_default, cb)); }
template <> inline bool FSM::ConditionalCallback<PeaPod::Menu::peapod_menustate_t>::compare(comparators_t cmp, const PeaPod::Menu::peapod_menustate_t& val, const PeaPod::Menu::peapod_menustate_t& ref) { return true; } // Every comparison is unequal, since we only use unconditional callbacks

#endif