# 1. Automation Subsystem Requirements Outline

## 1. Purpose

The Automation Subsystem shall coordinate sensing, actuation, control execution, data logging, communication, fault handling, and environment-program execution across all PeaPod subsystems.

## 2. Scope

### Included

* central controller;
* microcontroller/network architecture;
* subsystem module communication;
* environment-program execution;
* sensor polling;
* actuator command dispatch;
* fault detection;
* operational data logging;
* remote command reception;
* local status indication;
* safe-state management.

### Excluded

* physical generation of temperature, humidity, light, or nutrient delivery;
* crop-specific biological optimization logic unless allocated separately;
* cloud analytics beyond data transmission/export.

## 3. Parent Requirements Allocated

* remote operational data and limited video transmission;
* periodic operational command reception;
* reproducible time-series dataset generation;
* formal environment-control specification;
* automation;
* modular implementation;
* reliability and safety.

## 4. Key Interfaces

### Inputs

* sensor readings from all subsystems;
* user commands;
* remote commands;
* environment programs;
* calibration data;
* fault signals.

### Outputs

* actuator commands;
* subsystem setpoints;
* logged time-series data;
* alarms/fault states;
* operational telemetry;
* exported trial datasets.

### Interfaces to Other Subsystems

* Aeroponics: pump/valve commands, nutrient delivery data, reservoir state.
* Air Thermoregulation: temperature setpoints, heater/cooler/fan commands.
* Humidification: humidity setpoints, humidifier commands.
* Dehumidification: dehumidifier commands, condensate/fault data.
* Lighting: spectrum/intensity/photoperiod commands.
* Housing: door/cover state, leak detection, enclosure status.

## 5. Operating Modes

* Off
* Boot
* Hardware Discovery
* Idle
* Environment Program Loaded
* Active Trial
* Calibration
* Maintenance
* Fault
* Safe Shutdown

## 6. Functional Requirements

* AUTO-FUNC-001: The Automation Subsystem shall execute time-series environment programs.
* AUTO-FUNC-002: The Automation Subsystem shall assign each trial a unique trial identifier.
* AUTO-FUNC-003: The Automation Subsystem shall associate all logged subsystem data with the active trial identifier.
* AUTO-FUNC-004: The Automation Subsystem shall poll required sensors at defined sampling intervals.
* AUTO-FUNC-005: The Automation Subsystem shall dispatch actuator commands according to the active control program.
* AUTO-FUNC-006: The Automation Subsystem shall receive and apply permitted remote operational commands.
* AUTO-FUNC-007: The Automation Subsystem shall prevent unsafe actuator commands outside subsystem-defined limits.
* AUTO-FUNC-008: The Automation Subsystem shall detect missing, unresponsive, or invalid subsystem modules.
* AUTO-FUNC-009: The Automation Subsystem shall enter a defined safe state upon critical fault detection.

## 7. Performance Requirements

Define:

* minimum control-loop frequency;
* maximum command latency;
* sensor polling frequency;
* timestamp resolution;
* data-loss tolerance;
* uptime target;
* acceptable clock drift;
* module discovery time;
* remote command response time.

## 8. Safety Requirements

* fail lights off unless lighting is required for safe operation;
* fail pumps off unless required to prevent damage;
* prevent heater/cooler conflict;
* prevent humidification/dehumidification conflict unless explicitly allowed;
* prevent uncontrolled continuous misting;
* prevent command execution when enclosure state is unsafe;
* log all fault transitions.

## 9. Data Requirements

Required logs:

* trial ID;
* timestamp;
* active environment program ID/version;
* subsystem ID;
* sensor value;
* actuator command;
* actuator state if measurable;
* fault state;
* operator intervention;
* calibration version.

## 10. Verification

* Demonstrate loading and executing an environment program.
* Demonstrate sensor polling and actuator dispatch.
* Demonstrate data export with linked trial ID.
* Simulate subsystem disconnection.
* Simulate invalid sensor values.
* Demonstrate safe-state transition.