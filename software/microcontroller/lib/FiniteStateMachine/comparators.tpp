#ifndef FSM_COMPARATORS_H_
#error __FILE__ should only be included AFTER <comparators.h>
// #include <comparators.h>
#endif

#ifdef FSM_COMPARATORS_H_

#ifndef FSM_COMPARATORS_T_
#define FSM_COMPARATORS_T_

// #error COMPARATORS.TPP

#define BOOLSTR(b) (b ? "true" : "false")

#include <debug.h>

// TODO: All T args in callbacks -> const T&
// TODO: Replace all OneShot with Latching (duh)

template <typename T> ConditionalCallback<T>::ConditionalCallback(comparators_t cmp, const T& ref, const void* cb, callback_type_t cbtype, bool invert, cb_getref_t getReference) : comparator(cmp), reference(ref), callback(cb), callbacktype(cbtype), invert(invert), getReference(getReference) {
  #ifdef FSM_DEBUG_SERIAL
    DEBUG_DELAY();
    FSM_DEBUG_SERIAL.print(_F("Callback Created: X "));
    FSM_DEBUG_SERIAL.print(parseComparator(cmp));
    FSM_DEBUG_SERIAL.print(" ");
    FSM_DEBUG_SERIAL.print(ref);
    if(invert) FSM_DEBUG_SERIAL.print(_F(" (Inverted) "));
    if(getReference != nullptr) FSM_DEBUG_SERIAL.print(_F(" (Dynamic Ref) "));
    FSM_DEBUG_SERIAL.print("\n");
    DEBUG_DELAY();
  #endif
}

template <typename T> ConditionalCallback<T>::ConditionalCallback(comparators_t cmp, const T& ref, cb_none_t cb, bool invert, cb_getref_t getReference) : ConditionalCallback(cmp, ref, (const void*)cb, CB_NONE, invert, getReference) { }

template <typename T> ConditionalCallback<T>::ConditionalCallback(comparators_t cmp, const T& ref, cb_comp_t cb, bool invert, cb_getref_t getReference) : ConditionalCallback(cmp, ref, (const void*)cb, CB_COMP, invert, getReference) { }

template <typename T> ConditionalCallback<T>::ConditionalCallback(comparators_t cmp, const T& ref, cb_compval_t cb, bool invert, cb_getref_t getReference) : ConditionalCallback(cmp, ref, (const void*)cb, CB_COMPVAL, invert, getReference) { }

template <typename T> ConditionalCallback<T>::ConditionalCallback(comparators_t cmp, const T& ref, cb_compvalref_t cb, bool invert, cb_getref_t getReference) : ConditionalCallback(cmp, ref, (const void*)cb, CB_COMPVALREF, invert, getReference) { }

template <typename T> ConditionalCallback<T>::ConditionalCallback(comparators_t cmp, const T& ref, bool invert, cb_getref_t getReference) : ConditionalCallback(cmp, ref, nullptr, CB_COMP, invert, getReference) { }

template <typename T> ConditionalCallback<T>::ConditionalCallback(comparators_t cmp, const T& ref, const void* cb, callback_type_t cbtype, cb_getref_t getReference) : comparator(cmp), reference(ref), callback(cb), callbacktype(cbtype), getReference(getReference) {
  #ifdef FSM_DEBUG_SERIAL
    DEBUG_DELAY();
    FSM_DEBUG_SERIAL.print(_F("Callback Created: X "));
    FSM_DEBUG_SERIAL.print(parseComparator(cmp));
    FSM_DEBUG_SERIAL.print(" ");
    FSM_DEBUG_SERIAL.print(ref);
    if(getReference != nullptr) FSM_DEBUG_SERIAL.print(_F(" (Dynamic Ref) "));
    FSM_DEBUG_SERIAL.print("\n");
    DEBUG_DELAY();
  #endif
}

template <typename T> ConditionalCallback<T>::ConditionalCallback(comparators_t cmp, const T& ref, cb_none_t cb, cb_getref_t getReference) : ConditionalCallback(cmp, ref, (const void*)cb, CB_NONE, getReference) { }

template <typename T> ConditionalCallback<T>::ConditionalCallback(comparators_t cmp, const T& ref, cb_comp_t cb, cb_getref_t getReference) : ConditionalCallback(cmp, ref, (const void*)cb, CB_COMP, getReference) { }

template <typename T> ConditionalCallback<T>::ConditionalCallback(comparators_t cmp, const T& ref, cb_compval_t cb, cb_getref_t getReference) : ConditionalCallback(cmp, ref, (const void*)cb, CB_COMPVAL, getReference) { }

template <typename T> ConditionalCallback<T>::ConditionalCallback(comparators_t cmp, const T& ref, cb_compvalref_t cb, cb_getref_t getReference) : ConditionalCallback(cmp, ref, (const void*)cb, CB_COMPVALREF, getReference) { }

template <typename T> ConditionalCallback<T>::ConditionalCallback(comparators_t cmp, const T& ref, cb_getref_t getReference) : ConditionalCallback(cmp, ref, nullptr, CB_COMP, getReference) { }

template <typename T> ConditionalCallback<T>& ConditionalCallback<T>::operator=(ConditionalCallback<T> const& other) { return ConditionalCallback<T>(other.comparator, other.reference, other.callback, other.callbacktype, other.getReference); }

template <typename T> void ConditionalCallback<T>::childCallback(bool comp, const T& val, const T& ref) { return; }
template <typename T> const T& ConditionalCallback<T>::childReference(const T& val) { return this->reference; }

template <typename T> void ConditionalCallback<T>::operator()(const T& val) { 
  if(this->disabled) return;
  
  // Update the reference value
  if (this->getReference != nullptr) {
    this->reference = this->getReference(val);
    #ifdef FSM_DEBUG_SERIAL
      DEBUG_DELAY();
      FSM_DEBUG_SERIAL.print(_F("Got Reference: "));
      FSM_DEBUG_SERIAL.print(this->reference);
      FSM_DEBUG_SERIAL.print("\n");
      DEBUG_DELAY();
    #endif
  } else {
    this->reference = this->childReference(val);
    #ifdef FSM_DEBUG_SERIAL
      DEBUG_DELAY();
      FSM_DEBUG_SERIAL.print(_F("Got Child Reference: "));
      FSM_DEBUG_SERIAL.print(this->reference);
      FSM_DEBUG_SERIAL.print("\n");
      DEBUG_DELAY();
    #endif
  }
  this->callOperator(val, this->reference);
}

template <typename T> void ConditionalCallback<T>::operator()(const T& val, const T& ref) { this->callOperator(val, ref); }

template <typename T> void ConditionalCallback<T>::setComparator(comparators_t cmp) {
  #ifdef FSM_DEBUG_SERIAL
    DEBUG_DELAY();
    FSM_DEBUG_SERIAL.print(_F("Comparator Change: "));
    FSM_DEBUG_SERIAL.print(parseComparator(cmp));
    FSM_DEBUG_SERIAL.print("\n");
    DEBUG_DELAY();
  #endif
  this->comparator = cmp;
}

template <typename T> void ConditionalCallback<T>::callOperator(const T& val, const T& ref) {
  bool result = ConditionalCallback<T>::compare(this->comparator, val, ref);

  if(result) this->executeCallback(val, ref);
}

template <typename T> void ConditionalCallback<T>::executeCallback(const T& val, const T& ref) {
  #ifdef FSM_DEBUG_SERIAL
    DEBUG_DELAY();
    FSM_DEBUG_SERIAL.print(invert ? _F("Callback! (Inverted)") : _F("Callback!"));
    FSM_DEBUG_SERIAL.print("\n");
    DEBUG_DELAY();
  #endif

  bool _ = this->invert ? false : true;

  // #ifdef FSM_DEBUG_SERIAL
  //   DEBUG_DELAY();
  //   FSM_DEBUG_SERIAL.print("Passing ");
  //   FSM_DEBUG_SERIAL.print(_ ? "true" : "false");
  //   FSM_DEBUG_SERIAL.print("\n");
  //   DEBUG_DELAY();
  // #endif

  if(this->callback == nullptr) {
    // Assumes childCallback
    #ifdef FSM_DEBUG_SERIAL
      DEBUG_DELAY();
      FSM_DEBUG_SERIAL.print(_F("No Callback - Calling Child: "));
      DEBUG_DELAY();
    #endif
    this->childCallback(_, val, ref);
    return;
  };
  
  // Cast void* cb fn pointer to appropriate type and call
  switch(this->callbacktype) {
    case CB_NONE:
      // Called with no args
      ((cb_none_t)this->callback)();
      break;
    case CB_COMP:
      ((cb_comp_t)this->callback)(_);
      break;
    case CB_COMPVAL:
      ((cb_compval_t)this->callback)(_, val);
      break;
    case CB_COMPVALREF:
      ((cb_compvalref_t)this->callback)(_, val, ref);
      break;
  }
}

template <typename T> void ConditionalCallback<T>::disable() { this->disabled = true; }
template <typename T> void ConditionalCallback<T>::enable() { this->disabled = false; }
template <typename T> void ConditionalCallback<T>::enable(const T& val) { this->disabled = false; this->executeCallback(val, this->reference); }

#endif

#endif