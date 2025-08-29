#include <statelinker.h>

#include <debug.h>
#include <state.h>
#include <comparators.h>
#include <number.h>

BangBangFlagSet::BangBangFlagSet(const Number& lo, const Number& hi, Flag* flag) : low(Number::minimum(lo, hi)), high(Number::maximum(lo, hi)), FlagSetCondition<Number>(CMP_LES, low, flag, (double)hi < (double)lo) {
  #ifdef FSM_DEBUG_SERIAL
    DEBUG_DELAY();
    FSM_DEBUG_SERIAL.print(_F("BangBang FlagSet ["));
    FSM_DEBUG_SERIAL.print((double)lo);
    FSM_DEBUG_SERIAL.print(", ");
    FSM_DEBUG_SERIAL.print((double)hi);
    FSM_DEBUG_SERIAL.print("]\n");
    DEBUG_DELAY();
  #endif
}

void BangBangFlagSet::childCallback(bool comp, const Number& val, const Number& ref) {
  this->state = (ref == this->high);

  #ifdef FSM_DEBUG_SERIAL
    if(state) {
      DEBUG_DELAY();
      FSM_DEBUG_SERIAL.print(_F("BangBang (High)\n"));
      DEBUG_DELAY();
    } else {
      DEBUG_DELAY();
      FSM_DEBUG_SERIAL.print(_F("BangBang (Low)\n"));
      DEBUG_DELAY();
    }
  #endif

  // state ? comp : !comp;
  // T XOR T = T
  // F XOR T = F
  // T XOR F = F
  // F XOR F = T
  // comp = !(comp ^ this->state);
  // comp = this->state ? comp : !comp;
  bool _  = !(this->invert ^ this->state);

  #ifdef FSM_DEBUG_SERIAL
    DEBUG_DELAY();
    FSM_DEBUG_SERIAL.print(_F("Flag Set ("));
    FSM_DEBUG_SERIAL.print(_ ? "true" : "false");
    FSM_DEBUG_SERIAL.print(")\n");
    DEBUG_DELAY();
  #endif
  this->flag->set(_);
}

const Number& BangBangFlagSet::childReference(const Number& val) {
  #ifdef FSM_DEBUG_SERIAL
    DEBUG_DELAY();
    FSM_DEBUG_SERIAL.print(_F("BangBang State: "));
    FSM_DEBUG_SERIAL.print(BOOLSTR(this->state));
    FSM_DEBUG_SERIAL.print("\n");
    DEBUG_DELAY();
  #endif
  if (this->state && val <= this->low) {
    this->setComparator(CMP_LES);
    this->triggered = false;
    this->state = false;
    #ifdef FSM_DEBUG_SERIAL
      DEBUG_DELAY();
      FSM_DEBUG_SERIAL.print(_F("BangBang State: Expecting Low\n"));
      DEBUG_DELAY();
    #endif
  } else if (!this->state && val >= this->high) {
    this->setComparator(CMP_GTR);
    this->triggered = false;
    this->state = true;
    #ifdef FSM_DEBUG_SERIAL
      DEBUG_DELAY();
      FSM_DEBUG_SERIAL.print(_F("BangBang State: Expecting High\n"));
      DEBUG_DELAY();
    #endif
  }
  return this->state ? this->high : this->low;
}

void BangBangFlagSet::callOperator(const Number& val, const Number& ref) {
  bool result = compare(this->comparator, val, ref);
  if(!this->triggered && result) {
    this->triggered = true;
    this->executeCallback(val, ref);
  } else if(!result) {
    // Reset
    #ifdef FSM_DEBUG_SERIAL
      DEBUG_DELAY();
      FSM_DEBUG_SERIAL.print(_F("BangBang Trigger Reset!\n"));
      DEBUG_DELAY();
    #endif
    this->triggered = false;
  }
}

void LatchingFlagSet::callOperator(const bool& val, const bool& ref) {
  bool result = compare(this->comparator, val, ref);
  if(!this->triggered && result) {
    this->executeCallback(val, ref);
    this->triggered = true;
  } else if(!result) {
    // Reset
    #ifdef FSM_DEBUG_SERIAL
      DEBUG_DELAY();
      FSM_DEBUG_SERIAL.print(_F("Trigger Reset!\n"));
      DEBUG_DELAY();
    #endif
    this->triggered = false;
  }
}

LatchingFlagSet::LatchingFlagSet(Flag* flag, bool invert) : FlagSetCondition<bool>(CMP_EQU, !invert, flag, invert) { }


void LatchingFlagSet::childCallback(bool comp, const bool& val, const bool& ref) {
  this->state = !ref;
  this->triggered = false;

  #ifdef FSM_DEBUG_SERIAL
    if(val) {
      DEBUG_DELAY();
      FSM_DEBUG_SERIAL.print(_F("Latch (High)\n"));
      DEBUG_DELAY();
    } else {
      DEBUG_DELAY();
      FSM_DEBUG_SERIAL.print(_F("Latch (Low)\n"));
      DEBUG_DELAY();
    }
  #endif

  bool _ = val ^ ref;

  // Cast void* cb fn pointer to appropriate type and call
  #ifdef FSM_DEBUG_SERIAL
    DEBUG_DELAY();
    FSM_DEBUG_SERIAL.print(_F("Flag Set ("));
    FSM_DEBUG_SERIAL.print(_ ? "true" : "false");
    FSM_DEBUG_SERIAL.print(")\n");
    DEBUG_DELAY();
  #endif
  this->flag->set(_);
}

const bool& LatchingFlagSet::childReference(const bool& val) { return this->state; }