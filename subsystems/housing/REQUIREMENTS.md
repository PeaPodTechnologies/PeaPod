# 6. Housing Subsystem Requirements Outline

## 1. Purpose

The Housing Subsystem shall provide the physical enclosure, structure, containment, insulation, isolation, mounting, access, and modular physical interfaces required to maintain a controlled plant growth environment.

## 2. Scope

### Included

* frame;
* panels;
* doors/access points;
* seals;
* insulation;
* internal mounting features;
* root-zone/leaf-zone physical separation if applicable;
* light isolation;
* water containment;
* leak paths;
* cable/fluid pass-throughs;
* structural modularity.

### Excluded

* active temperature generation;
* active humidity generation/removal;
* active lighting;
* active nutrient delivery;
* control software except housing sensors such as door/leak sensors.

## 3. Parent Requirements Allocated

* environmental isolation;
* insulation;
* water-tightness;
* light containment;
* gas exchange/leak control;
* physical safety;
* size/volume;
* modularity;
* maintainability;
* documentation completeness.

## 4. Key Interfaces

### Inputs

* mounted subsystems;
* structural loads;
* thermal loads;
* water leaks;
* operator access;
* plant trays or growth modules;
* cables/tubes.

### Outputs

* enclosed controlled volume;
* structural support;
* isolation from surroundings;
* access state data if instrumented;
* leak state data if instrumented.

### Interfaces to Other Subsystems

* Automation: door state, leak sensors if present.
* Aeroponics: reservoir/root chamber mounting, leak containment.
* Air Thermoregulation: insulation, airflow paths.
* Humidification/Dehumidification: condensation surfaces and drainage.
* Lighting: mounting, reflection, light isolation.
* All subsystems: mechanical mounting and service access.

## 5. Operating Modes

* Open/Maintenance
* Closed/Ready
* Active Growth
* Cleaning
* Transport/Assembly
* Fault/Leak Containment

## 6. Functional Requirements

* HOUSE-FUNC-001: The subsystem shall physically enclose the plant growth environment.
* HOUSE-FUNC-002: The subsystem shall provide access for planting, inspection, harvest, cleaning, and maintenance.
* HOUSE-FUNC-003: The subsystem shall support mounting of all required subsystems.
* HOUSE-FUNC-004: The subsystem shall limit uncontrolled water escape from the plant growth environment.
* HOUSE-FUNC-005: The subsystem shall limit uncontrolled light exchange with the external environment.
* HOUSE-FUNC-006: The subsystem shall limit uncontrolled thermal exchange with the external environment.
* HOUSE-FUNC-007: The subsystem shall provide documented pass-throughs for electrical, data, air, and fluid interfaces.
* HOUSE-FUNC-008: The subsystem shall allow replacement of structural panels or members without full system redesign.
* HOUSE-FUNC-009: The subsystem shall fit within defined dimensional and volume constraints.

## 7. Performance Requirements

Define:

* outer dimensions;
* internal usable grow volume;
* maximum outer volume;
* structural load capacity;
* leak containment volume;
* water leak rate;
* air exchange/leak rate;
* heat loss;
* light leakage;
* cleaning access clearance;
* maximum assembly time;
* maximum number of tools required;
* door/opening dimensions;
* panel replacement time.

## 8. Safety Requirements

* no sharp exposed edges during normal use;
* stable against tipping under normal loading;
* contain expected water leaks;
* use materials compatible with humidity and cleaning;
* prevent unsafe access to active electrical components;
* support safe lifting/transport if applicable;
* prevent uncontrolled door opening if required.

## 9. Data Requirements

Required logs, if instrumented:

* door/access state;
* leak detection state;
* enclosure temperature if separate from air-control sensors;
* maintenance access events;
* panel replacement events;
* cleaning events.

## 10. Verification

* Measure dimensions and volume.
* Demonstrate subsystem installation.
* Demonstrate access for planting/harvest/maintenance.
* Test water containment.
* Test light isolation.
* Test thermal loss.
* Inspect modular replacement procedures.
* Demonstrate assembly/disassembly procedure.