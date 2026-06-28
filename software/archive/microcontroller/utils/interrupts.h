#ifndef PEAPOD_UTILS_INTERRUPTS_H_
#define PEAPOD_UTILS_INTERRUPTS_H_

#include <Arduino.h>

typedef void (*interruptHandler)(void);

class InterruptHandler {
  public:
    /**
     * Constructor.
     * @param pin Digital pin to attach interrupt to
     */
    InterruptHandler(const uint8_t* pin);

    /**
     * Destructor. Used to detach the interrupt
     */
    ~InterruptHandler();

    /**
     * Attaches the interrupt to trigger `handleInterrupt` on THIS instance
     * @param mode Mode (i.e. RISING)
     */
    void attachInstanceInterrupt(int mode);

    /**
     * Triggered by interrupt. Triggers handlerTrigger
     */
    void handlerTrigger(void);

    /** 
     * Triggered by handlerTrigger. To be implemented by the child class.
     */
    virtual void handleInterrupt(void) = 0;
  private:
    const uint8_t* pin;
};

#endif