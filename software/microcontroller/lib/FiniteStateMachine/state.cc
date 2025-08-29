#define FSM_SRC 1

#include <state.h>

#include <statelinker.h>
#include <comparators.h>
#include <number.h>

#ifdef FSM_DEBUG_SERIAL

template <> void stateDebug<Number>(const Number& val) {
  if(val.isNaN()) {
    FSM_DEBUG_SERIAL.print("NaN");
  } else {
    FSM_DEBUG_SERIAL.print(val.operator double());
  }
}

template <> void stateDebug<bool>(const bool& val) {
  FSM_DEBUG_SERIAL.print(val ? "true" : "false");
}

template <> void stateDebug<fsm_timestamp_t>(const fsm_timestamp_t& val) {
  FSM_DEBUG_SERIAL.print(val);
}

#endif

template <> ConditionalCallback<Number>* State<Number>::addLoggerCallback(typename ConditionalCallback<Number>::cb_compval_t cb) { return addConditional(new ConditionalCallback<Number>(CMP_NEQ, Number(), cb)); }

template <> ConditionalCallback<fsm_timestamp_t>* State<fsm_timestamp_t>::addLoggerCallback(typename ConditionalCallback<fsm_timestamp_t>::cb_compval_t cb) { return addConditional(new ConditionalCallback<fsm_timestamp_t>(CMP_NEQ, (fsm_timestamp_t)-1, cb)); }

template <> ConditionalCallback<bool>* State<bool>::addLoggerCallback(typename ConditionalCallback<bool>::cb_compval_t cb) { return addConditional(new ConditionalCallback<bool>(CMP_NOP, false, cb)); }

template <> ConditionalCallback<Number>* State<Number>::addLatchingSetFlag(const Number& on, const Number& off, Flag* flag) {
  // if(on < off) {
    // "heater"
    return addConditional(new BangBangFlagSet(on, off, flag));
  //   addConditional(oneshot ? new LatchFlagSet<Number>(CMP_LES, on, flag) : new FlagSetCondition<Number>(CMP_LES, on, flag));
  //   addConditional(oneshot ? new LatchFlagSet<Number>(CMP_GTR, off, flag, true) : new FlagSetCondition<Number>(CMP_GTR, off, flag, true));
  // } else {
    // "cooler"
    // return addConditional(new BangBangFlagSet(off, on, flag));
  //   addConditional(oneshot ? new LatchFlagSet<Number>(CMP_GTR, on, flag) : new FlagSetCondition<Number>(CMP_GTR, on, flag));
  //   addConditional(oneshot ? new LatchFlagSet<Number>(CMP_LES, off, flag, true) : new FlagSetCondition<Number>(CMP_LES, off, flag, true));
  // }
}

template <> ConditionalCallback<Number>* State<Number>::addLatchingConditional(const Number& on, const Number& off, const void* cb, callback_type_t cbtype) {
  // if(on < off) {
    // "heater"
    return addConditional(new BangBangConditional(on, off, cb, cbtype));
    // addConditional(oneshot ? new OneShotConditional<Number>(CMP_LES, on, cb, cbtype) : new ConditionalCallback<Number>(CMP_LES, on, cb, cbtype));
    // addConditional(oneshot ? new OneShotConditional<Number>(CMP_GTR, off, cb, cbtype, true) : new ConditionalCallback<Number>(CMP_GTR, off, cb, cbtype, true));
  // } else {
    // "cooler"
    // return addConditional(new BangBangConditional(off, on, cb, cbtype));
    // addConditional(oneshot ? new OneShotConditional<Number>(CMP_GTR, on, cb, cbtype) : new ConditionalCallback<Number>(CMP_GTR, on, cb, cbtype));
    // addConditional(oneshot ? new OneShotConditional<Number>(CMP_LES, off, cb, cbtype, true) : new ConditionalCallback<Number>(CMP_LES, off, cb, cbtype, true));
  // }
}

template <> ConditionalCallback<fsm_timestamp_t>* State<fsm_timestamp_t>::addLatchingSetFlag(const fsm_timestamp_t& on, const fsm_timestamp_t& off, Flag* flag) {
  return (ConditionalCallback<fsm_timestamp_t>*)(new BangBangFlagSet(Number(on), Number(off), flag));
}

template <> ConditionalCallback<fsm_timestamp_t>* State<fsm_timestamp_t>::addLatchingConditional(const fsm_timestamp_t& on, const fsm_timestamp_t& off, const void* cb, callback_type_t cbtype) {
  return (ConditionalCallback<fsm_timestamp_t>*)(new BangBangConditional(Number(on), Number(off), cb, cbtype));
}

// BOOLEAN HYSTERESIS = STEP CHANGE FILTER

template <> ConditionalCallback<bool>* State<bool>::addLatchingConditional(const bool& on, const bool& off, const void* cb, callback_type_t cbtype) {
  return addConditional(new LatchingConditional(cb, cbtype, (char)on < (char)off));
}

template <> ConditionalCallback<bool>* State<bool>::addLatchingSetFlag(const bool& on, const bool& off, Flag* flag) {
  return addConditional(new LatchingFlagSet(flag, (char)on < (char)off));
}

Variable::Variable(const Number& value) : State<Number>(value) { }
Variable::Variable(const double& value) : Variable(Number(value)) { }
Variable::Variable(const float& value) : Variable(Number(value)) { }
Variable::Variable(const long& value) : Variable(Number(value)) { }
Variable::Variable(const int& value) : Variable(Number((long)value)) { }
Variable::Variable(const unsigned long& value) : Variable(Number(value)) { }
Variable::Variable(const unsigned int& value) : Variable(Number((unsigned long)value)) { }

Flag::Flag(const bool& value) : State<bool>(value) { }