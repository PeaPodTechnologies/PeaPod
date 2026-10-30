# Lighting Subsystem Requirements

## 1. Purpose

The lighting subsystem shall provide programmable plant-growth lighting with controlled intensity, spectrum, photoperiod, and spatial distribution suitable for reproducible phenotype dataset generation and food production.

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

* Automation: commands, schedules, telemetry, imaging.
* Housing: mounting, reflectivity, light isolation.
* Air Thermoregulation: heat load coordination.
* Humidification/Dehumidification: condensation protection.

## 5. Operating Modes

* LIGHT-MODE-001: Off
* LIGHT-MODE-002: Standby
* LIGHT-MODE-003: Active
* LIGHT-MODE-004: Sanitization
* LIGHT-MODE-005: Imaging
* LIGHT-MODE-006: Fault

## 6. Requirements

* LIGHT-REQ-001: MUST provide controlled light to the plant canopy.
* LIGHT-REQ-002: MUST support programmable photoperiods (on/off).
* LIGHT-REQ-003: MUST support programmable light intensity (dimming).
* LIGHT-REQ-004: MUST support programmable spectral channels.
* LIGHT-REQ-005: SHOULD report commanded and active lighting states.
* LIGHT-REQ-006: SHOULD support sanitization via disinfection light if allocated.
* LIGHT-REQ-007: SHOULD support imaging light if allocated.
* LIGHT-REQ-008: MUST expose documented swappable lighting components.
* LIGHT-REQ-009: SHOULD define its heat output or thermal load for the Air Thermoregulation Subsystem.

## 7. Criteria

* LIGHT-CRIT-001: PPFD range at canopy height;
* LIGHT-CRIT-002: spectral wavelength/channel range;
* LIGHT-CRIT-003: intensity stability;
* LIGHT-CRIT-004: spatial uniformity;
* LIGHT-CRIT-005: dimming resolution;
* LIGHT-CRIT-006: photoperiod (on/off) timing accuracy;
* LIGHT-CRIT-007: maximum power draw;
* LIGHT-CRIT-008: energy efficiency;
* LIGHT-CRIT-009: thermal output;
* LIGHT-CRIT-010: expected LED lifetime.