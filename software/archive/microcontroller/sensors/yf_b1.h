#ifndef PEAPOD_SENSORS_YF_B1_H_
#define PEAPOD_SENSORS_YF_B1_H_

// HEADERS

#include <Arduino.h>

#include <utils/base.h>
#include <sensors/sensor.h>
#include <utils/interrupts.h>

// MACROS

// Settings
#define YF_B1_DELTA         1000  // Calculate frequency every second
#define YF_B1_COUNT_PER_LPM 7.5   // 7.5 counts is one liter per minute

// GLOBALS
// Used when you need a pointer to static PROGMEM - these variables are ONLY INITIALIZED in the .cc SOURCE file that INCLUDES this header file
#ifndef GLOBALS // Double-declaration blocker
  #define GLOBALS
  // Descriptors
  static const PROGMEM char* const id = "YF-B1";

  // Data setup
  static const PROGMEM char* const labels[1] = { "water_flow_rate" };
  static const PROGMEM SensorDataSetup datasetup = {
    .numdata = 1,
    .labels = labels
  };
#endif

// CLASS

// Interface for the Seeed Studio YF-B1 flow sensor.
class YF_B1 : public Sensor, public InterruptHandler {
  public:
    /**
     * Constructor.
     * @param pin Digital input pin. MUST be capable of interrupts (pins 2 and 3 on the Arduino Nano)
     */
    YF_B1(const uint8_t pin);
  private:
    errorlevel_t initialize(void) override;

    errorlevel_t read(float* data, uint8_t numdata) override;

    // Digital input pin.
    const uint8_t pin;

    // Triggered by interrupt. Increments flow counter.
    void handleInterrupt(void) override;

    // Flow interrupt counter - volatile for interrupt compatibility
    volatile uint32_t flow_count;

    // Timestamp of last flowrate calculation
    unsigned long last_count;
};

#endif
