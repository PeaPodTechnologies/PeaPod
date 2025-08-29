#ifndef I2CIP_DEVICE_H_
#error __FILE__ should only be included AFTER <device.h>
// #include <comparators.h>
#endif

#ifdef I2CIP_DEVICE_H_

#ifndef I2CIP_DEVICE_T_
#define I2CIP_DEVICE_T_

#include <debug.h>

using I2CIP::i2cip_errorlevel_t;
using I2CIP::InputGetter;
using I2CIP::OutputSetter;
using I2CIP::InputInterface;
using I2CIP::OutputInterface;
using I2CIP::IOInterface;

template <typename G, typename A> InputInterface<G, A>::InputInterface(Device* device) { device->setInput(this); }

template <typename G, typename A> InputInterface<G, A>::~InputInterface() { }

template <typename G, typename A> G InputInterface<G, A>::getCache(void) const { return this->cache; }

template <typename G, typename A> void InputInterface<G, A>::setCache(G value) { this->cache = value; }

template <typename G, typename A> void InputInterface<G, A>::clearCache(void) {
  // #ifdef I2CIP_DEBUG_SERIAL
  //   DEBUG_DELAY();
  //   I2CIP_DEBUG_SERIAL.print(F("InputInterface::clearCache() Not Implemented; Nothing Done\n"));
  //   DEBUG_DELAY();
  // #endif
}

template <typename G, typename A> void InputInterface<G, A>::setArgsA(A args) { this->argsA = args; }

template <typename G, typename A> A InputInterface<G, A>::getArgsA(void) const { return this->argsA; }

template <typename G, typename A> i2cip_errorlevel_t InputInterface<G, A>::get(const void* args) { 
  #ifdef I2CIP_DEBUG_SERIAL
    DEBUG_DELAY();
    I2CIP_DEBUG_SERIAL.print(F("InputInterface::get("));
    if(args == &InputGetter::failptr_get) I2CIP_DEBUG_SERIAL.print(F("fail"));
    else if(args == nullptr) I2CIP_DEBUG_SERIAL.print(F("null"));
    else { 
      I2CIP_DEBUG_SERIAL.print(*(A* const)args);
      I2CIP_DEBUG_SERIAL.print(F(" @0x"));
      I2CIP_DEBUG_SERIAL.print((uint16_t)args, HEX);
    }
    I2CIP_DEBUG_SERIAL.print(F(")\n"));
    DEBUG_DELAY();
  #endif

  if (args == &InputGetter::failptr_get) this->clearCache();
  G temp = this->cache;

  if(!this->argsAset) { this->argsA = this->getDefaultA(); this->argsAset = true; }
  A arg = (args == &InputGetter::failptr_get) ? this->getDefaultA() : ((args == nullptr) ? this->getArgsA() : *(A* const)args);

  i2cip_errorlevel_t errlev = this->get(temp, arg);

  // If successful, update last cache
  if(errlev == I2CIP::i2cip_errorlevel_t::I2CIP_ERR_NONE) { 
    // #ifdef I2CIP_DEBUG_SERIAL
    //   DEBUG_DELAY();
    //   I2CIP_DEBUG_SERIAL.print(F(" -> "));
    //   I2CIP_DEBUG_SERIAL.print(temp);
    //   I2CIP_DEBUG_SERIAL.print(F(" @0x"));
    //   I2CIP_DEBUG_SERIAL.print((uint16_t)temp, HEX);
    //   I2CIP_DEBUG_SERIAL.print(F(", setting cache\n"));
    //   DEBUG_DELAY();
    // #endif
    // this->clearCache(); 
    this->cache = temp; this->argsA = arg; 
  }
  return errlev;
}

template <typename S, typename B> OutputInterface<S, B>::OutputInterface(Device* device) { device->setOutput(this); }

template <typename S, typename B> OutputInterface<S, B>::~OutputInterface() { }

template <typename S, typename B> void OutputInterface<S, B>::setValue(S value) { this->value = value; }

template <typename S, typename B> void OutputInterface<S, B>::resetFailsafe(void) {
  // #ifdef I2CIP_DEBUG_SERIAL
  //   DEBUG_DELAY();
  //   I2CIP_DEBUG_SERIAL.print(F("OutputInterface::resetFailsafe() Not Implemented; Nothing Done\n"));
  //   DEBUG_DELAY();
  // #endif
}

template <typename S, typename B> S OutputInterface<S, B>::getValue(void) const { return this->value; }

template <typename S, typename B> void OutputInterface<S, B>::setArgsB(B args) { this->argsB = args; }

template <typename S, typename B> B OutputInterface<S, B>::getArgsB(void) const { return this->argsB; }

template <typename S, typename B> i2cip_errorlevel_t OutputInterface<S, B>::set(const void* value, const void* args) {
  #ifdef I2CIP_DEBUG_SERIAL
    DEBUG_DELAY();
    I2CIP_DEBUG_SERIAL.print(F("OutputInterface::set("));
    if(value == &OutputSetter::failptr_set) I2CIP_DEBUG_SERIAL.print(F("fail"));
    else if(value == nullptr) I2CIP_DEBUG_SERIAL.print(F("null"));
    else { 
      I2CIP_DEBUG_SERIAL.print(*(S* const)value);
      I2CIP_DEBUG_SERIAL.print(F(" @0x"));
      I2CIP_DEBUG_SERIAL.print((uint16_t)value, HEX);
    }
    I2CIP_DEBUG_SERIAL.print(F(", "));
    if(args == &OutputSetter::failptr_set) I2CIP_DEBUG_SERIAL.print(F("fail"));
    else if(args == nullptr) I2CIP_DEBUG_SERIAL.print(F("null"));
    else { 
      I2CIP_DEBUG_SERIAL.print(*(B* const)args);
      I2CIP_DEBUG_SERIAL.print(F(" @0x"));
      I2CIP_DEBUG_SERIAL.print((uint16_t)args, HEX);
    }
    I2CIP_DEBUG_SERIAL.print(F(")\n"));
    DEBUG_DELAY();
  #endif

  // If fail, reset to failsafe value
  if (value == &OutputSetter::failptr_set) this->resetFailsafe();
  // 1. If `set` value is not given, repeat last action
  S val = ((value == nullptr || value == &OutputSetter::failptr_set) ? this->getValue() : *(S* const)value);

  if(!this->argsBset) { this->argsB = this->getDefaultB(); this->argsBset = true; }

  // 2. If `set` args are not given, use last args 
  B arg = (args == &OutputSetter::failptr_set) ? this->getDefaultB() : ((args == nullptr || value == nullptr) ? this->getArgsB() : *(B* const)args);

  // 3. Attempt `set`
  i2cip_errorlevel_t errlev = this->set(val, arg);

  // 4. If successful, update cached `value` and `args`
  if(errlev == I2CIP::i2cip_errorlevel_t::I2CIP_ERR_NONE) { this->value = val; this->argsB = arg; };

  return errlev;
}

template <typename G, typename A, typename S, typename B> IOInterface<G, A, S, B>::IOInterface(Device* device) : InputInterface<G, A>(device), OutputInterface<S, B>(device) { }

template <typename G, typename A, typename S, typename B> IOInterface<G, A, S, B>::~IOInterface() { }

#endif
#endif