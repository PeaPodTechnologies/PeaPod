#include <utils/interrupts.h>

#include <Arduino.h>

InterruptHandler::InterruptHandler(const uint8_t* pin) {
  this->pin = pin;
}

InterruptHandler::~InterruptHandler() {
  detachInterrupt(digitalPinToInterrupt(*pin));
}

void InterruptHandler::attachInstanceInterrupt(int mode) {
  // Pointer-to-member-function &InterruptHandler::handleInterrupt points to handleInterrupt on THIS instance
  // and is then converted to a normal function pointer to its place in memory
  // This causes a `-Wpmf-conversions` warning, but works so long as the object instance outlives the interrupt
  // This is ensured by detaching the interrupt on object destruction
  attachInterrupt(digitalPinToInterrupt(*pin), (interruptHandler)(&InterruptHandler::handlerTrigger), RISING);
}

void InterruptHandler::handlerTrigger(void) {
  this->handleInterrupt();
}