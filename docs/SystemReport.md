# PeaPod System-Level Report

## Housing

**Purpose**: *Isolates* and *insulates* growth environment from surroundings (heat, light, water vapour, air). Provides structural integrity and mounting points for other subsystems, and enables system extendability via repeated "unit cell" topology.

**Method**:

 - *Setup*:
    1.  Assemble frame and insert panels;
    2.  Mount control module (w/ subsystems), connect inputs;
    3.  Install tray mounts, insert trays (w/ subsystems);

 - *Testing*:
    - Frame construction is rigid, level, and sturdy.
    - Panels are insulating against temperature changes, and mitigate water vapour loss.

 - *Process*:

    1.  Panels insulate against heat gain/loss, are opaque, and contain light and heat via reflection;
    2.  Shell construction is tight, thus sealing against moisture;
    3.  Internal vertical mounting channels for systems and horizontal plane "trays";
    4.  Self-contained control module with all subsystem supplies, as well as automation systems;
    5.  Solenoid lock to prevent unintended opening;
    6.  **Housing Extension** (can be repeated):
        1.  Add a second housing;
        2.  Remove dividing panel from both housings;
        3.  Join the two housings to form one larger 2x1 housing;
        4.  **Extension Modes** (may be combined in any way to suit
            application):
            - *Class 1* (no combined units, frame connection only): Leave the dividing panel, add a control module, and operate the two PeaPods **separately**.
            - *Class 2* (for 2-4 unit housing combinations): Operate the combined housing off **one** control module.
            - *Class 3* (for 5+ unit housing combinations): Add control modules to account for additional air volume, plant count, power requirement, etc. and operate in a **master-slaves topology**.

**Features**:

- *Control Unit*: Top-mounted self-contained unit encapsulating all system inputs (incl. power, water, network connection), subsystem supplies and controls (incl. power supplies, aeroponics supply i.e. pump and pressure vessel and solenoid, thermoregulation control, humidity control, gas composition and exchange, and automation systems).

- *Frame*: T-slotted 2020 aluminum extrusion framing with face-mounted brackets forms a cubic skeleton for rigidity/strength (high strength-to-weight aluminum) and easy component mounting and repositioning. These extrusions form the edges of the cubic housing.

- *Panels*: Graphite-enhanced expanded polystyrene (GPS) rigid foam insulation panels with reflective mylar internal lamination increase energy efficiency (GPS RSI of 0.0328$\frac{m^2 \cdot \degree C}{W}$ per mm of thickness, mylar enables light/heat reflection), as well as safety against cross-contamination and pathogens. Panels press-fit into the frame and form a seal for greater water vapour retention.

- *Solenoid Lock*: Normally-open solenoid lock engages on-demand to prevent unintended contact with environment. Controlled by a relay. Mitigates cross-contamination and maintains environment accuracy.

- *Grow Trays*: Horizontal plane subframes mounted to internal vertical extrusion channels for ease of leveling and repositioning. Trays slide in/out on drawer slides mounted to front-back-horizontal DIN rails. All electrical and water connections are quick-disconnect for ease of tray removal.

- *Lighting Units*: Arrays of LED strips mounted to left-right-horizontal DIN rails (which also support top-down cameras).

## Aeroponics

**Purpose**: Delivers plant nutrients and pH- and temperature-controlled water to the roots via a fine mist.

**Function**:
- **Inputs**: Reverse osmosis water under positive pressure (pre-dosed with nutrient solutions and pH-balanced), nozzle delivery on/off control, water thermoregulation control signal
- **Outputs**: pH- and nutrient-balanced water mist (50 micron mean droplet diameter)

**Method**:

 - *Setup*:
    1.  Connect water input to the supply unit;
    2.  Connect supply unit to the supply pump input;
    3.  Connect runoff recapture line to the supply unit (for recycling) or to drain;
    4.  Connect the quick-disconnect fittings to the grow tray;
    5.  Calibrate pressure, temperature sensors;
    6.  Enable water input to prime system;
    7.  Mount aeroponic container to tray, ensure drawstring seal is fitted to the extrusion channel;

 - *Testing*:
    - Temperature, pressure sensors communicate as expected.
    - No leaks at any component under a) source pressure, b) fully pressurized.
    - Supply pump actuates and auto-shuts off as expected, and is able to deliver the required pressure.
    - All components, tubing, and connectors/fittings withstand full pressurization.
    - Solenoid is normally closed, withstands full pressurization, and opens when power is applied.
    - Quick-disconnect operates as intended at full pressurization without leaks.
    - Nozzles produce even-distribution full-cone mist.
    - Runoff container is sealed, and runoff collection operates as intended.

 - *Process*:
    1.  Water source fills supply unit (manual);
    2.  Nutrient solutions and pH balancers are dosed into supply unit as
        necessary (manual);
    3.  Heat is added to the water;
    4.  Water is pressurized to constant 80psi;
    5.  Temperature and pressure of the water is read (feedback);
    6.  Flow to nozzle is controlled (on/off);
    7.  Nozzle turns pressurized water into mist;
    8.  Runoff is contained by a water-tight container, and recaptured for
        recycling or draining;

**Features**:

- *Water Source* (EXTERNAL DEPENDENCY): A source of reverse osmosis water.
- *Supply Unit*: A coated-steel bucket with a lid holds reverse osmosis water for dosing with nutrient solutions and pH-balancing. Features an adhesive-back heat pad with thermostat.
- *Supply Pump*: Self-priming diaphragm pump, auto-shutoff at 80psi. Power is controlled by a relay.
- *Water Temperature Sensor*: Tee-fitted. Informs the heat pad thermostat.
- *Pressure Vessel*: Uses an air bladder to maintain and stabilize pressure.
- *Pressure Sensor*: Allows for shutoff of pump in case of emergency.
- *Drain Valve*: Tee-fitted ball valve. Allows the system to be depressurized and drained.
- *Solenoid Valve*: Controls delivery to the nozzles to enable on-demand misting.
- *Grow Tray Quick-Disconnect*: One each for watering line and runoff recapture line. Enables quick disconnection with auto-shutoff so the trays may be removed.
- *Nozzle*: Mounted to underside of grow tray, pointed at plant roots. 80psi water through a 0.4-0.6mm orifice produces ~50 micron water droplets, optimal for plant growth.
- *Root-Zone Container*: Watertight container that encapsulates the entire root zone. Made of a woven waterproof composite fabric (CT5K.18 mylar with Dyneema, 33.89g/m^2), chosen for high strength-to-weight ratio (15x that of steel) and natural no-coating food-safe waterproof quality. Mounted and **sealed** to the grow tray with a drawstring for easy root zone access.
- *Runoff Collection Pump*: Small sump pump to recapture runoff water for recycling or draining. Auto-activated when water is detected in the root-zone container (built-in float switch).

## Leaf-Zone Thermoregulation

**Purpose**: Maintaining desired leaf-zone air temperature and circulating air.

**Function**:
- **Inputs**: Power, air temperature control signal, air circulation control signal
- **Outputs**: Heat to/from system, by-product heat from/to surroundings, internal air circulation, internal air temperature sensor readings

**Method**:

 - *Testing*:

    - Heat pump direction and magnitude respond to control signal as expected;
    - Fans operate as expected;
    - Heat pump power exceeds maximum heat transfer at system temperature extremes;
    - Heat pump power exceeds that required to reach temperature extremes in under 120 seconds given the system’s heat capacity;

 - *Process*:

    1.  Air is circulated throughout the environment;
    2.  Temperature is measured, sent to automation system;
    3.  Control module controls heat pump speed and direction (heating vs. cooling system);

**Features**:

- *Circulation Fans*: Located in growth environment to circulate air for even temperature distribution, rapid system flushing, and automatic pollination.
- *Temperature Sensors*: Multiple temperature and humidity sensors frame-mounted throughout the growth environment to measure air temperature. Informs the **PID control loop**.
- *Heat Pump*: Pumps heat in or out of the growth environment. Is comprised of:
  - *Peltier Device*: 85W bidirectional solid-state **thermoelectric device** (aka Peltier tile) pumps heat from one face to the other. Better space efficiency, less complexity (no liquids, pressurized fluids, etc.), and more precise than other methods.
  - *Thermoelectric Driver Board*: Controls *magnitude* and *direction* of heat transfer via a **dimmable voltage source** (low-pass-filtered PWM to a voltage buffer and amplifier w/ feedback) and **relay H-bridge**, respectively.
  - *Heat Sinks*: Aluminum blocks with fins hold and exchange heat between air and Peltier devices. One set on each side of the Peltier (inside and outside environment) builds "heat pump". Mating face coated with thermal compound for better transfer.
  - *Heat Sink Fans*: Located on both sets of heat sinks for better heat dissipation.
- *PID Control Loop*: A propotional-integral-derivative control loop enables increased accuracy. Temperature sensors inform the loop, "error" is calculated (current vs desired temperature), and this informs the magnitude and direction of heat pump control. Requires tuning of parameters. Built into the automation system.

## Leaf-Zone Humidity Regulation

**Purpose**: Regulates the relative humidity of the leaf zone.

**Function**:
- **Inputs**: Humidification on/off control signal, dehumidification on/off control signal
- **Outputs**: Humidification or dehumidification on-demand

**Method**:

 - *Process*:
    1.  When humidity is too low (below dead-zone), humidification is activated;
    2.  When humidity is too high (above dead-zone), dehumidification is activated;
    3.  When humidity is at target (within dead-zone), both systems are deactivated;

**Features**:

- *Humidification System*
- *Dehumidification System*
- *Humidity Sensors*: Multiple temperature and humidity sensors frame-mounted throughout the growth environment to measure air relative humidity (%RH). Informs the **bang-bang control loop**.
- *Bang-Bang Control Loop*: A bang-bang (on/off) control loop with a hysteresis dead-zone. Humidity sensors inform the loop, "error" is calculated (current vs desired humidity), and this informs whether or not to activate either the humidification or dehumidification systems (or neither). Requires tuning of dead-zone (automatic). Built into the automation system.


### Humidification System

**Purpose**: Actively *increases* growth environment air humidity.

**Function**:
- **Inputs**: Power, humidification on/off control signal, reverse osmosis water
- **Outputs**: Dry water vapour

**Method**:

 - *Setup*:
    1.  Connect humidification control signal to control module.
    2.  Fill water tank.

 - *Testing*:
    - Humidification unit responds to control signal as expected;
    - Humidity sensor reads as expected;
    - Tank does not leak;

 - *Process*:
    1.  Water is delivered to a small tank (nebulizer is mounted);
    2.  Power and control signal activate a nebulizer driver;
    3.  Nebulizer vapourizes water;

**Features**:

- *Circulation Fans*: To circulate dry water vapour for even humidification.
- *Humidification Unit*: Easily controllable and produces a consistent vapour. Comprised of:
  - *Water Tank*: Holds a small amount of water behind the piezoelectric mesh.
  - *Mesh Nebulizer*: Piezoelectric ceramic disc with a microporous stainless steel mesh in the center. Oscillates in such a way that dry vapour is generated when water is passed over the mesh. Mounted to the water tank.
  - *Driver Circuit*: Fixed-frequency 555 timer circuit driving an amplifier/LC circuit generates an sinusoidal signal. Powers the piezoelectric disc.

### Dehumidification System

**Purpose**: Actively *decreases* growth environment air humidity.

**Function**:
- **Inputs**: Humid air (high water vapour content), dehumidification on/off control signal, dry desiccant
- **Outputs**: Dry air (low water vapour content), saturated desiccant, desiccant saturation level signal

**Method**:

 - *Setup*:
    1.  Connect dehumidification control signal to control module;
    2.  Insert dry desiccant cartridge;

 - *Testing*:
    - Desiccant removes moisture from air.
    - Desiccant indicates saturation as expected, which is sensed by computer.
    - Shutters operate as intended, and no dehumidification occurs when closed.
    - Maximum dehumidification rate exceeds total plant transpiration rate.

 - *Process*:
    1.  Dehumidification control signal activates fans and opens shutters;
    2.  Humid air passes over the desiccant, and dry air exits the unit;
    3.  Desiccant becomes saturated, and indicates degree of saturation;
    4.  Indication is sensed by computer, which notifies the user when to replace and dehydrate/"recharge" desiccant;

**Features**:

- *Dehumidification Unit*: One input port and one output port. Comprised
  of:
  - *Fans*: Humidity-rated fans force moist air through the desiccant cartridge input port and dry air out of the output port.
  - *Filter*: Polyethylene-polyropylene blend (non-toxic) MERV 13 (0.3 micron) air filters located at input and output ports of dehumidification chamber eliminate risk of any airborne pathogens being transferred onto silica beads and out of the system during cartridge recharging.
  - *Shutters*: Servo-actuated shutters enable opening and closing of dehumidifier input and output on demand. Air-tight when closed to prevent unintended dehumidification.
  - *Desiccant Cartridge*: Oven-safe. Easily removable for swapping and "recharging". Contains the silica gel desiccant.
  - *Indicating Silica Gel Desiccant*: Cheap, efficient, food-safe, reusable chemical desiccant beads with a water mass capacity of 6%. Changes color from blue to pink when saturated.
- *Color Sensor*: Optical color sensor senses cartridge saturation. Informs when to recharge the desiccant cartridge.
- *Evaporator Oven* (EXTERNAL DEPENDENCY): A ventilated oven that can maintain 125°C for 12 hours. Heats cartridge to evaporate/"bake off" moisture collected by silica beads, thus "recharging" them.

## Gas Composition Regulation and Exchange

**Purpose**: Controls gas composition of the growth environment by mediating exchange with surroundings.

**Function**:
- **Inputs**: Power, exchange control signal (open/closed and exchange rate)
- **Outputs**: Gas intake (from surroundings), gas exhaust (to surroundings; filtered and humidity-controlled)

**Method**:

 - *Setup*:
    1.  Connect exhaust port to onboard filtration/dehumidification system;
    2.  Connect shutter servos, fans to control module;

 - *Testing*:
    - Shutter servos, fans operate as intended.
    - Ports are air-tight when closed.
    - Exhaust filter removes all aerosols (i.e. pollen, seeds) and pathogens.
    - Exhaust dehumidification brings humidity down to ambient (~60%).

 - *Process*:
    1.  On-demand, intake and exhaust ports activate. Shutters open, and fans are enabled;
    2.  Intake port draws in air from surroundings;
    3.  Exhaust port expels air through filtration and dehumidification systems to be recycled;

**Features**:

- *Exchange Port*: Intake and exhaust, normally sealed. Each comprises:
  - *Shutters*: Servo-actuated shutters enable opening and closing of ports on demand. Air-tight when closed.
  - *Fan*: Humidity-rated fans control gas intake and exhaust rates.
  - *Filter*: Polyethylene-polyropylene blend (non-toxic) MERV 13 (0.3 micron) air filters eliminate risk of any airborne pathogens being transferred into or out of the system during gas exchange.

- *Gas Concentration Sensors*: A variety of sensors frame-mounted in the growth environment collect data on concentrations (ppm) of relevant gasses (CO2, O2, etc.). Informs the **bang-bang control loops**.
- *Bang-Bang Control Loops*: Maintains gas concentrations to within desired ranges.
- *Output Dehumidifier*: **Onboard life support systems** provides a dehumidifier (as well as additional filtration) to mitigate exhaust humidity.

## Lighting

**Purpose**: Discrete light spectrum and intensity control to provide all light necessary for plant growth, as well as sanitization.

**Function**:
- **Inputs**: Power, lighting spectrum-intensity control signal (aka per-LED modulation signals)
- **Outputs**: Spectrum and intensity controlled light

**Method**:

 - *Setup*:
    1.  Connect power and spectrum-intensity control signal to LED board onboard drivers.
    2.  Mount LED boards to lighting unit.

 - *Testing*:
    - Spectrum-intensity distribution control signal modulates LED power as expected;
    - Passive heat sinks dissipate enough heat;

 - *Process*:
    1.  Power is delivered to drivers;
    2.  Control signals "dim" drivers to modulate intensity distribution across spectrum;
    3.  Power drivers power LEDs;
    4.  LEDs emit light;

**Features**:

- *LED Lights*: LEDs offer high power output, better efficiency and thermal management, lower footprint, and precise wavelengths while minimizing risk of damaging plant tissues. Many discretely-controlled wavelength options/"series" enable wide and fine control of intensity-spectrum distribution, with a focus on Photosynthetically-Active Radiation (PAR), as well as sanitization wavelengths and wavelengths to induce specific phenotypic and chemical changes. Located across multiple smaller daisy-chained PCBs to minimize cost. LED series include:
  - Ultraviolet (267nm) for sanitization;
  - Blue (448nm);
  - Cool White (5700K);
  - Warm White (2700K);
  - Red (645nm);
  - Near-Infrared (730nm);

- *LED Power Drivers*: High-efficiency constant-current PWM-dimmable
  DC-DC buck converters, specialized for LEDs. One per series, driving
  a set of identical LEDs. One driver per LED board.