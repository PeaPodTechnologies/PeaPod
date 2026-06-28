# 3. Air Thermoregulation Subsystem Requirements Outline

## 1. Purpose

The Air Thermoregulation Subsystem shall measure and control leaf-zone air temperature within the plant growth environment according to the active environment program.

## 2. Scope

### Included

* air temperature sensing;
* heating elements;
* cooling elements if present;
* heat exchangers if present;
* thermal circulation devices directly required for temperature control;
* insulation-relevant thermal interface requirements;
* thermal safety cutoffs;
* temperature-control feedback.

### Excluded

* humidity control except thermal side effects;
* nutrient solution temperature unless allocated;
* lighting heat generation except as a disturbance input;
* housing insulation design except interface requirements.

## 3. Parent Requirements Allocated

* leaf-zone thermoregulation;
* environment parameter control;
* automation;
* plant growth environment containment;
* energy efficiency;
* safety;
* reproducibility.

## 4. Key Interfaces

### Inputs

* temperature setpoint;
* ambient temperature disturbance;
* lighting heat load;
* plant/transpiration heat effects;
* air circulation state;
* power.

### Outputs

* heat addition;
* heat removal;
* temperature telemetry;
* fault states;
* thermal energy consumption data.

### Interfaces to Other Subsystems

* Automation: setpoints, sensor data, actuator commands.
* Housing: insulation, airflow paths, enclosure leakage.
* Lighting: heat load coordination.
* Humidification/Dehumidification: temperature/humidity coupling.
* Aeroponics: root-zone evaporation and water temperature effects.

## 5. Operating Modes

* Off
* Standby
* Heating
* Cooling
* Hold/Maintain
* Calibration
* Fault
* Thermal Safe Shutdown

## 6. Functional Requirements

* THERM-FUNC-001: The subsystem shall measure leaf-zone air temperature.
* THERM-FUNC-002: The subsystem shall control leaf-zone air temperature according to a commanded setpoint.
* THERM-FUNC-003: The subsystem shall support programmable setpoint changes over time.
* THERM-FUNC-004: The subsystem shall report measured temperature to the Automation Subsystem.
* THERM-FUNC-005: The subsystem shall prevent unsafe heating or cooling operation.
* THERM-FUNC-006: The subsystem shall detect invalid or missing temperature sensor readings.
* THERM-FUNC-007: The subsystem shall enter a safe state upon over-temperature detection.
* THERM-FUNC-008: The subsystem shall expose replaceable heater, cooler, sensor, and airflow components where applicable.

## 7. Performance Requirements

Define:

* controlled temperature range;
* steady-state tolerance;
* spatial uniformity requirement;
* response rate;
* overshoot limit;
* sensor accuracy;
* sensor resolution;
* sampling frequency;
* maximum power draw;
* heat loss limit;
* recovery time after door opening or disturbance.

## 8. Safety Requirements

* over-temperature cutoff;
* heater fail-off behavior;
* safe clearance from plant material and housing surfaces;
* no exposed hot surfaces accessible during normal operation;
* no condensation damage caused by cooling components;
* thermal runaway prevention;
* fault logging.

## 9. Data Requirements

Required logs:

* air temperature readings;
* temperature setpoints;
* heater/cooler command state;
* heater/cooler measured state if available;
* thermal fault events;
* calibration records;
* energy consumption estimate.

## 10. Verification

* Test temperature range.
* Test steady-state control tolerance.
* Test response to setpoint step changes.
* Test sensor failure behavior.
* Test over-temperature shutdown.
* Analyze energy consumption.