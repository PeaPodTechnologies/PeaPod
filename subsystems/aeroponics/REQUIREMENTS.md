# 2. Aeroponics Subsystem Requirements Outline

## 1. Purpose

The Aeroponics Subsystem shall deliver water and nutrient solution to plant roots in a controlled, repeatable, low-waste manner suitable for plant growth trials and food production.

## 2. Scope

### Included

* reservoir;
* pump;
* tubing;
* filters;
* nozzles/misters;
* root-zone delivery hardware;
* nutrient solution delivery control;
* root-zone moisture delivery;
* leak containment related to nutrient solution;
* flow sensing if included;
* reservoir level sensing if included.

### Excluded

* closed-loop chemical dosing unless explicitly included;
* plant lighting;
* air temperature control;
* air humidity control except humidity generated as a side effect;
* biological crop selection.

## 3. Parent Requirements Allocated

* minimal inputs;
* minimal water waste;
* plant growth input provision;
* nutrient solution control;
* root-zone support;
* automation;
* safety;
* reproducibility;
* modularity.

## 4. Key Interfaces

### Inputs

* nutrient solution;
* water;
* power;
* pump commands;
* valve/nozzle commands;
* delivery schedule;
* reservoir refill/maintenance input.

### Outputs

* mist or droplets to root zone;
* water/nutrient usage data;
* reservoir level data;
* flow data;
* leak/fault states;
* waste/return flow if applicable.

### Interfaces to Other Subsystems

* Automation: control commands and logging.
* Housing: root chamber mounting, containment, drainage/leak path.
* Humidification/Dehumidification: moisture load interaction.
* Air Thermoregulation: nutrient solution temperature interaction.
* Lighting: crop growth dependency but no direct control responsibility.

## 5. Operating Modes

* Off
* Prime
* Ready
* Active Delivery
* Flush
* Drain
* Cleaning
* Fault
* Maintenance

## 6. Functional Requirements

* AERO-FUNC-001: The Aeroponics Subsystem shall deliver nutrient solution to the plant root zone.
* AERO-FUNC-002: The Aeroponics Subsystem shall support configurable delivery schedules.
* AERO-FUNC-003: The Aeroponics Subsystem shall support a commanded delivery rate or delivery duration.
* AERO-FUNC-004: The Aeroponics Subsystem shall prevent continuous uncontrolled nutrient delivery.
* AERO-FUNC-005: The Aeroponics Subsystem shall detect or mitigate reservoir empty conditions.
* AERO-FUNC-006: The Aeroponics Subsystem shall detect or mitigate leaks that could escape the contained root-zone volume.
* AERO-FUNC-007: The Aeroponics Subsystem shall allow cleaning or replacement of wetted components.
* AERO-FUNC-008: The Aeroponics Subsystem shall expose documented fluidic and mechanical interfaces for replacement.

## 7. Performance Requirements

Define:

* nutrient delivery rate range;
* delivery rate tolerance;
* minimum droplet/mist coverage requirement;
* maximum daily water waste;
* reservoir capacity;
* priming time;
* maximum allowable leak rate;
* nozzle clog tolerance;
* pump duty cycle;
* nutrient solution temperature compatibility;
* component cleaning interval.

## 8. Safety Requirements

* prevent overflow;
* prevent uncontrolled spray outside root chamber;
* prevent electrical exposure to leaks;
* prevent stagnant inaccessible wetted areas;
* ensure food-contact or plant-contact materials are appropriate;
* support safe cleaning procedures;
* fail to a non-flooding state.

## 9. Data Requirements

Required logs:

* delivery start/stop timestamps;
* commanded delivery duration;
* pump/valve state;
* reservoir level;
* flow rate if measured;
* water added;
* nutrient solution batch ID;
* fault events;
* cleaning events;
* component replacement events.

## 10. Verification

* Measure delivery rate at defined command values.
* Demonstrate delivery repeatability.
* Demonstrate no uncontrolled delivery after power loss or fault.
* Demonstrate reservoir empty response.
* Demonstrate leak containment.
* Inspect cleanability and replaceability.