#ifndef FSM_STATE_H_
#error __FILE__ should only be included AFTER <state.h>
// #include <state.h>
#endif
#ifndef FSM_STATELINKER_H_
#error __FILE__ should only be included AFTER <statelinker.h>
// #include <statelinker.h>
#endif

#ifdef FSM_STATE_H_
#ifdef FSM_STATELINKER_H_

#ifndef FSM_STATE_T_
#define FSM_STATE_T_

// #error STATE.TPP

#include <debug.h>

template <typename T> State<T>::State(const T& value) : value(value), defaultValue(value) { }
template <typename T> State<T>::State(const T& value, const char* key) : value(value), defaultValue(value), key(key) { }

template <typename T> T State<T>::get(void) {
  return this->value;
}

template <typename T> void State<T>::set(const T& val) {
  #ifdef FSM_DEBUG_SERIAL
    DEBUG_DELAY();
    if(this->key == nullptr) {
      FSM_DEBUG_SERIAL.print(_F("State Set: "));
    } else {
      FSM_DEBUG_SERIAL.print(this->key);
      FSM_DEBUG_SERIAL.print(_F(" Set: "));
    }
    stateDebug<T>(val);
    FSM_DEBUG_SERIAL.print("\n");
    FSM_DEBUG_SERIAL.print(_F("==== [ Conditionals ] ===="));
    FSM_DEBUG_SERIAL.print("\n");
    DEBUG_DELAY();
  #endif

  if(!this->disabled) {
    for(unsigned int i = 0; i < MAX_CONDITIONALS; i++) {
      if(this->conditionals[i] == nullptr) break;

      this->conditionals[i]->operator()(val);
    }
  }

  #ifdef FSM_DEBUG_SERIAL
    DEBUG_DELAY();
    FSM_DEBUG_SERIAL.print(_F("==== [ End of Conditionals"));
    if(this->key != nullptr) {
      FSM_DEBUG_SERIAL.print(": ");
      FSM_DEBUG_SERIAL.print(this->key);
    }
    FSM_DEBUG_SERIAL.print(_F(" ] ====\n"));
    DEBUG_DELAY();
  #endif

  this->value = val;
}

template <typename T> ConditionalCallback<T>* State<T>::addConditional(ConditionalCallback<T>* conditional) {
  for (unsigned char i = 0; i < MAX_CONDITIONALS; i++) {
    if(this->conditionals[i] == nullptr) {
      this->conditionals[i] = conditional;
      return this->conditionals[i];
    }
  }
  return nullptr;
}

template <typename T> ConditionalCallback<T>* State<T>::addConditional(comparators_t cmp, const T& ref, typename ConditionalCallback<T>::cb_none_t cb, typename ConditionalCallback<T>::cb_getref_t getReference) { return addConditional(new ConditionalCallback<T>(cmp, ref, cb, getReference)); }

template <typename T> ConditionalCallback<T>* State<T>::addConditional(comparators_t cmp, const T& ref, typename ConditionalCallback<T>::cb_comp_t cb, typename ConditionalCallback<T>::cb_getref_t getReference) { return addConditional(new ConditionalCallback<T>(cmp, ref, cb, getReference)); }

template <typename T> ConditionalCallback<T>* State<T>::addConditional(comparators_t cmp, const T& ref, typename ConditionalCallback<T>::cb_compval_t cb, typename ConditionalCallback<T>::cb_getref_t getReference) { return addConditional(new ConditionalCallback<T>(cmp, ref, cb, getReference)); }

template <typename T> ConditionalCallback<T>* State<T>::addConditional(comparators_t cmp, const T& ref, typename ConditionalCallback<T>::cb_compvalref_t cb, typename ConditionalCallback<T>::cb_getref_t getReference) { return addConditional(new ConditionalCallback<T>(cmp, ref, cb, getReference)); }

template <typename T> ConditionalCallback<T>* State<T>::addConditionalSetFlag(comparators_t cmp, const T& ref, Flag* flag, typename ConditionalCallback<T>::cb_getref_t getReference) { return addConditional(new FlagSetCondition<T>(cmp, ref, flag, getReference)); }

template <typename T> ConditionalCallback<T>* State<T>::addConditionalSetFlag(comparators_t cmp, const T& ref, Flag* flag, bool invert, typename ConditionalCallback<T>::cb_getref_t getReference) { return addConditional(new FlagSetCondition<T>(cmp, ref, flag, invert, getReference)); }

// OTHER ARGS

template <typename T> ConditionalCallback<T>* State<T>::addLatchingConditional(const T& on, const T& off, typename ConditionalCallback<T>::cb_comp_t cb) { return addLatchingConditional(on, off, (const void*)cb, CB_COMP); }

template <typename T> ConditionalCallback<T>* State<T>::addLatchingConditional(const T& on, const T& off, typename ConditionalCallback<T>::cb_compval_t cb) { return addLatchingConditional(on, off, (const void*)cb, CB_COMPVAL); }

template <typename T> ConditionalCallback<T>* State<T>::addLatchingConditional(const T& on, const T& off, typename ConditionalCallback<T>::cb_compvalref_t cb) { return addLatchingConditional(on, off, (const void*)cb, CB_COMPVALREF); }

template <typename T> bool State<T>::isDisabled() { return this->disabled; }
template <typename T> void State<T>::enable() { this->disabled = false; }
template <typename T> void State<T>::enable(const T& val) { this->disabled = false; this->set(val); }
template <typename T> void State<T>::disable() { this->disabled = true; }
template <typename T> void State<T>::disable(const T& val) { this->set(val); this->disabled = true; }
template <typename T> void State<T>::resume() { this->disabled = false; this->set(this->value); }


#endif
#endif
#endif