# 5. Dehumidification Subsystem Requirements Outline

## 1. Purpose

The Dehumidification Subsystem shall remove water vapor from the plant growth environment to maintain commanded leaf-zone relative humidity and prevent unsafe condensation or excessive humidity.

## 2. Scope

### Included

* dehumidification device;
* condensate collection or drainage;
* humidity sensing if allocated;
* dehumidification control;
* condensation management;
* fault detection;
* maintenance access.

### Excluded

* humidification;
* nutrient solution delivery;
* primary air temperature control except thermal side effects;
* enclosure sealing except interface requirements.

## 3. Parent Requirements Allocated

* leaf-zone humidity control;
* contamination reduction;
* process safety;
* water containment;
* environment stability;
* reproducibility;
* automation.

## 4. Key Interfaces

### Inputs

* RH setpoint;
* air stream;
* power;
* condensate storage/drain path;
* enclosure leakage/disturbance conditions.

### Outputs

* reduced air humidity;
* condensate;
* heat load if applicable;
* fault states;
* telemetry.

### Interfaces to Other Subsystems

* Automation: commands and data logging.
* Housing: condensate containment and mounting.
* Humidification: opposing-control lockout/coordination.
* Air Thermoregulation: heat/cooling coupling.
* Lighting: condensation protection.
* Aeroponics: moisture load compensation.

## 5. Operating Modes

* Off
* Standby
* Active Dehumidification
* Hold
* Condensate Full
* Defrost/Dry-Out if applicable
* Maintenance
* Fault

## 6. Functional Requirements

* DEHUM-FUNC-001: The subsystem shall reduce leaf-zone relative humidity when commanded.
* DEHUM-FUNC-002: The subsystem shall support programmable humidity setpoints.
* DEHUM-FUNC-003: The subsystem shall collect or route condensate without leakage into unsafe areas.
* DEHUM-FUNC-004: The subsystem shall detect or mitigate condensate-full conditions.
* DEHUM-FUNC-005: The subsystem shall prevent simultaneous uncontrolled humidification and dehumidification.
* DEHUM-FUNC-006: The subsystem shall report operating state and fault conditions.
* DEHUM-FUNC-007: The subsystem shall support cleaning or replacement of condensate-contact components.

## 7. Performance Requirements

Define:

* RH reduction rate;
* RH control tolerance;
* maximum maintained RH;
* condensate capacity;
* condensate leak rate;
* energy use;
* thermal side-effect limit;
* noise/vibration if relevant;
* maintenance interval.

## 8. Safety Requirements

* prevent condensate overflow;
* prevent condensate contact with electrical components;
* prevent mold-promoting stagnant water;
* fail safe when condensate container is full;
* log excessive humidity events;
* prevent unsafe cooling surface condensation near plants or electronics.

## 9. Data Requirements

Required logs:

* RH readings;
* RH setpoints;
* dehumidifier command state;
* condensate level/full events;
* condensate volume if measured;
* fault events;
* cleaning events;
* energy consumption estimate.

## 10. Verification

* Test RH reduction from high-humidity condition.
* Test steady-state RH control.
* Test condensate-full response.
* Test leakage containment.
* Test interaction with humidification subsystem.
* Inspect cleanability and replaceability.