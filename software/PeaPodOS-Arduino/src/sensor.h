#ifndef PEAPOD_SENSOR_H_
#define PEAPOD_SENSOR_H_

#include "Arduino.h"

#include "base.h"

// Sensor identifiers
typedef enum sensorid_t {
  SENSOR_SHT31,
  SENSOR_K30,
} sensorid_t;

// Single datapoint
typedef struct t_datapoint {
  // Value
  float value;

  // Dataset label (string literal)
  const char* label;
} t_datapoint;

// All sensor state info + latest datum - used as the "cache" in the sensor object.
typedef struct SensorState {
  // Error level
  errorlevel_t error;

  // Other state information
  debuglevel_t debug;

  // Last data update (millis since program start) - NOT THE SAME AS `lastread`, an unsuccessful read attempt will NOT update `state.timestamp`.
  uint32_t timestamp;

  // Array of datapoints
  t_datapoint* data;

  // Length of `data` array
  uint8_t numdata;
} SensorState;

typedef struct t_sensordatasetup {
  // Number of datasets recorded by this sensor
  const uint8_t numdata;

  // Array of string literals corresponding to the labels for each dataset
  const char** labels;
} t_sensordatasetup;

class Sensor {
  public:
    /**
     * Sensor interface constructor.
     * @param sensor Identifier
     * @param setup Pointer to datasets setup
     * @param delta Minimum delay (in milliseconds) between sensor read attempts
     */
    Sensor(sensorid_t sensorid, const t_sensordatasetup* setup, uint32_t delta);
    
    /** Wrapper function for `initialize()`. Sets debug state to indicate initialization success or failure.
     * @return Pointer to sensor state
     */
    SensorState* begin(void);

    /** 
     * Wrapper function for `read()`. Checks debug state (initialized), ensures `delta` compliance, and sets `lastread` and `data` flags on read success.
     * @return Pointer to sensor state
     */
    SensorState* update(void);

    // @return Pointer to sensor state
    SensorState* getState(void);

    // @return Sensor identifier
    sensorid_t getID(void);

  protected:
    /** 
     * Initializes sensor. To be implemented by the child class.
     * @return Error level for this initialization attempt
     */
    virtual errorlevel_t initialize(void) = 0;

    /**
     * Reads data from sensor. To be implemented by the child class.
     * @param data The data array to be read into
     * @param numdata Length of the data array
     * @return Error level for this read attempt
     */
    virtual errorlevel_t read(float* data, uint8_t numdata) = 0;
    
  private:
    // Stores all the latest state data for this sensor.
    SensorState state;
  
    // Sensor identifier
    sensorid_t sensorid;

    // Minimum delay (in milliseconds) between sensor read attempts
    uint32_t delta;

    // Last read attempt (milliseconds since program start) - NOT THE SAME AS `state.timestamp`, an unsuccessful read attempt will still update `lastread`.
    uint32_t lastread;
};

#endif
