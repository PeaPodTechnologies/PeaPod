# 7. Lighting Subsystem Requirements Outline

## 1. Purpose

The Lighting Subsystem shall provide programmable plant-growth lighting with controlled intensity, spectrum, photoperiod, and spatial distribution suitable for reproducible growth trials and food production.

## 2. Scope

### Included

* light sources;
* drivers;
* dimming/control electronics;
* spectral channels;
* mounting;
* optical surfaces directly part of lighting;
* light intensity sensing if allocated;
* safety interlocks if allocated;
* photoperiod execution.

### Excluded

* housing-level light sealing except interface requirements;
* plant selection;
* air thermoregulation except lighting heat output as a disturbance;
* imaging lights unless included in the growth-light system.

## 3. Parent Requirements Allocated

* lighting control;
* plant growth input provision;
* environment program execution;
* palatability/variety support;
* energy efficiency;
* reproducibility;
* process safety;
* modularity.

## 4. Key Interfaces

### Inputs

* light program;
* intensity setpoints;
* spectral channel setpoints;
* photoperiod schedule;
* power;
* thermal limits;
* door/open state if interlocked.

### Outputs

* photons to canopy;
* heat load;
* light-state telemetry;
* power consumption;
* fault states.

### Interfaces to Other Subsystems

* Automation: commands, schedules, telemetry.
* Housing: mounting, reflectivity, light isolation.
* Air Thermoregulation: heat load coordination.
* Humidification/Dehumidification: condensation protection.
* Aeroponics: crop growth dependency but no direct control responsibility.

## 5. Operating Modes

* Off
* Standby
* Active Photoperiod
* Dimming/Ramp
* Calibration
* Maintenance
* Fault
* Safe-Off

## 6. Functional Requirements

* LIGHT-FUNC-001: The subsystem shall provide controlled light to the plant canopy.
* LIGHT-FUNC-002: The subsystem shall support programmable photoperiods.
* LIGHT-FUNC-003: The subsystem shall support programmable light intensity.
* LIGHT-FUNC-004: The subsystem shall support programmable spectral channels if multi-channel lighting is included.
* LIGHT-FUNC-005: The subsystem shall report commanded and active lighting states.
* LIGHT-FUNC-006: The subsystem shall prevent unsafe light operation during maintenance access if required.
* LIGHT-FUNC-007: The subsystem shall expose documented replaceable lighting components.
* LIGHT-FUNC-008: The subsystem shall define its heat output or thermal load for the Air Thermoregulation Subsystem.

## 7. Performance Requirements

Define:

* PPFD range at canopy height;
* spectral wavelength/channel range;
* intensity stability;
* spatial uniformity;
* dimming resolution;
* photoperiod timing accuracy;
* ramp-rate capability;
* maximum power draw;
* energy efficiency;
* thermal output;
* expected LED lifetime;
* calibration interval.

## 8. Safety Requirements

* prevent eye/skin exposure to hazardous light levels during maintenance;
* prevent electrical hazards in high-humidity conditions;
* prevent overheating of light drivers or LED boards;
* fail lights off on critical fault;
* prevent unsafe UV operation unless interlocked;
* log lighting faults.

## 9. Data Requirements

Required logs:

* photoperiod start/stop timestamps;
* commanded intensity;
* commanded spectral channel values;
* measured intensity if available;
* lighting power state;
* estimated power consumption;
* thermal derating events;
* fault events;
* calibration records.

## 10. Verification

* Measure PPFD range.
* Measure spatial uniformity.
* Measure spectral output or channel response.
* Test photoperiod timing.
* Test dimming resolution.
* Test safe-off behavior.
* Inspect replaceability and interface documentation.
