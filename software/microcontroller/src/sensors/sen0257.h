#ifndef PEAPOD_SENSORS_SEN0257_H_
#define PEAPOD_SENSORS_SEN0257_H_

#include <Arduino.h>

#include <utils/base.h>
#include <sensors/sensor.h>

#define SEN0257_DELTA 100
#define SEN0257_VMIN  0.5   // Min output voltage
#define SEN0257_VMAX  4.5   // Max output voltage
#define SEN0257_PMIN  0.0   // Min output pressure (PSI)
#define SEN0257_PMAX  261.1 // Max output pressure (PSI)

// Interface for the SenseAir K30 Extended Range CO2 sensor.
class SEN0257 : public Sensor {
  public:
    /**
     * Constructor.
     * @param pin Analog input pin.
     */
    SEN0257(uint8_t pin);
  private:
    errorlevel_t initialize(void) override;

    errorlevel_t read(float* data, uint8_t numdata) override;

    // Analog input pin.
    uint8_t pin;
};

#endif
