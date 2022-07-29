#ifndef PEAPOD_ACTUATORS_LED_H_
#define PEAPOD_ACTUATORS_LED_H_

// HEADERS

#include <Arduino.h>

#include <utils/base.h>
#include <actuators/actuator.h>
#include <actuators/pwm.h>

// GLOBALS
// Used when you need a pointer to static PROGMEM - these variables are ONLY INITIALIZED in the .cc SOURCE file that INCLUDES this header file
#ifndef GLOBALS // Double-declaration blocker
  #define GLOBALS
  // Descriptors - overallocated for suffix
  static const PROGMEM char* const id = "LED";
#endif

// CLASS

// Interface for PWM-dimmable LEDs.
class LED : public PWM {
  public:
    typedef enum ledwavelength_t {
      LED_CREE_XPG3_BLUE = 448,
      LED_CREE_XPG3_RED = 645,
      LED_CREE_XPE2_FARRED = 730,
    } ledwavelength_t;

    typedef enum ledtemperature_t {
      LED_CREE_XPG3_WARMWHITE = 2700,
      LED_CREE_XPG3_COOLWHITE = 5700,
    } ledtemperature_t;

    /**
     * Constructor.
     * @param pin PWM pin for controlling this LED.
     * @param wavelength Wavelength of light emitted (nm)
     */
    LED(const uint8_t pin, ledwavelength_t wavelength);

    /**
     * Constructor.
     * @param pin PWM pin for controlling this LED.
     * @param temperature Color temperature (K)
     */
    LED(const uint8_t pin, ledtemperature_t temperature);

    String toString(void);

  private:
    // LED color.
    const String color;
};

#endif
