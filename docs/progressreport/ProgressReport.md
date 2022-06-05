# Design Status

## Completion

The design is 85% complete as of May 31st, 2022. All high-level design
is complete, and most of what remains is to select specific components
for a few subsystems. A highly-accurate 3D model was created to select
and validate placement, orientation, and fit of all components prior to
prototyping.

## Process Description

### Setup

**Assembly**:
1.  Assemble housing, trays;
2.  Mount control module, trays;
3.  Connect subsystems to control module:
    -   *Housing Solenoid Lock*: relay
    -   *Aeroponics Supply & Runoff Collection Lines*: quick-disconnect
    -   *Lighting Driver Board*: power, control signal
4.  Connect gas exchange exhaust to ventilation

**Prepare Inputs**:
1.  Connect supply inputs:
    -   *Power*: 120V 60Hz AC
    -   *Network*: ethernet or wireless, optional
    -   *Water*: reverse-osmosis, ambient
2.  Insert consumable inputs:
    -   *Nutrient/pH Adjusment Solutions*: pouches
    -   *Dehumidification Cartridge*: recharged

**Initialize Growth Cycle**:
1.  Select or upload program;
2.  Insert seeds/clones;
3.  Seal and lock environment;
4.  Start growth cycle;

**Proceed to Operation**

### Operation

**Sense, Plan, Act** represents the three simultaneous automatic processes of the program's execution.
-   **Sense**: Environment data, diagnostic information, and timelapse photos are captured and stored at regular intervals.
-   **Plan**:
    -   *Control Systems*: Actuator controls are calculated from sensor readings and program targets.
    -   *Actuated Instructions*: Actuator controls are derived from program instructions.
-   **Act**: Subsystem actuators are controlled according to plan. Crew is notified to refill consumable inputs when low, change consumable inputs on program target change, and on any subsystem failure (see Maintenance), as well as when to harvest and on End of Program (EoP).

**Harvest and/or EoP**:
1.  Exhaust all air (automatic);
2.  Flush aeroponics system with pure water (automatic);
3.  Run UV disinfection (automatic);
4.  Unlock and open environment;
5.  Remove trays and detach aeroponic container;
6.  **If EoP**: Extract plants;
7.  Harvest and clean yield;
8.  Collect, clean, and store seeds/clones;
9.  **If EoP**: Dispose of inedible matter;

**If EoP, proceed to Cleaning. Otherwise, seal and lock
environment and resume program.**

### Maintenance

**Notification Handling**:
-   *Refill Consumable Inputs*: includes refilling nutrient/pH adjustment solutions, recharging the dehumidification cartridge
-   *Change Consumable Inputs*: includes changing nutrient/pH adjustment solutions
-   *Subsystem Failure*: all operation stopped, proceed to *Shutdown* (see Cleaning), disconnect power, then drain aeroponics system if necessary and replace failed components

### Cleaning {#sec:process_cleaning}

**Shutdown**:
1.  Remove nutrient/pH solutions, dehumidification cartridge;
2.  Attach aeroponic container and insert trays;
3.  Flush aeroponic system with descaling solution;
4.  Flush aeroponic system with pure water;
5.  Remove trays and detach aeroponic container;

**Cleaning**:
1.  Visually inspect all surfaces for debris (remove) and components for damage (replace);
2.  Visually inspect all air filters, replace if necessary;
3.  Wipe down all surfaces with soap solution;
4.  Wipe down all surfaces with pure water;

**Sanitization**:
1.  Wipe down all surfaces with disinfectant solution;
2.  Wipe down all surfaces with pure water;
3.  Dry all surfaces;
4.  Attach aeroponic container and insert trays;
5.  Flush aeroponic system with disinfectant solution;
6.  Flush aeroponic system with pure water;
7.  Seal and lock environment;
8.  Run UV sanitization;

**Proceed to Prepare Inputs** (see Setup).

# Prototype Build Status

## Completion

The prototype is 50% complete as of May 31st, 2022. The housing, aeroponics supply system (without dosage pumps or thermoregulation), lighting, and automation systems are fully operational.

## Successes, Results, and Products

As of May 31st, 2022, no food has been produced by PeaPod. However, we predict successful production of a variety of plant-based food products over the coming weeks, derived from a variety of plant types (leafy greens, legumes, garden vegetables, microgreens, root vegetables, herbs) and prepared using a variety of methods and combinations (i.e. meals). This is possible despite prototype incompleteness, as the remainder of subsystems (except nutrient/pH adjustment solution dosage) are non-critical to plant growth when ambient temperature, humidity, and gas composition are relatively suitable for plant growth at typical indoor ambient atmospheric conditions. In addition, this will still produce usable data for optimization, as all sensors are in place and collecting data even though their associated control systems are absent.

## Challenges

Processes for pollination and germination are still unclear, though simple solutions are possible (air circulation for pollination, seed
planting/germination in-place).

Due to the high current draw of the Peltier device (8.5A), as well as the necessity of voltage control over PWM, a custom driver circuit needed to be designed (as opposed to an off-the-shelf integrated circuit). The multi-stage driver (PWM to step-up MOSFETs to low-pass filter to operational amplifier voltage buffer to Darlington amplifier with feedback to relay H-bridge) was non-intuitive to design, and required many revisions (choosing Darlington over MOSFET due to op amp output current limits, choosing relays over MOSFETs due to variable supply voltage).

Specific component placement and orientation for extended housing topologies (as well as the process of actually extending a housing) is still unclear.

Selection of an aeroponics system type was a key design challenge, specifically around the ability to deliver mineral-rich water without calcification at high water delivery rates with suitable droplet diameter, which ultimately led to our selection of high-pressure nozzle-based aeroponics over piezoelectric mesh nebulizer aeroponics (as is used in some commercial settings).

Initially, the method of nutrient and pH-adjustment solution injection was Venturi siphon based. This presented a number of key issues, mainly controlling injection rate/mixing proportions while preventing backflow under pressure, which led us to pivot to peristaltic pumps, which have proven to be superior in both aspects.

The lighting system PCBs went through 4 revisions before consistently successful functionality was achieved. Reasons include pin alignment during LED board daisy-chaining, thermal management, and LED power limits.

The fitting selection for the aeroponics supply system was fully redesigned twice, first because of new component selection, and then again for part count/mass optimization. The aeroponics supply system also leaked frequently prior to optimizing the assembly process (proper securing of tapered and parallel thread mating faces using PTFE tape and bonded O-rings, respectively, as well as proper tightening).

The software has been restructured and portions completely rewritten countless times. Establishing and maintaining reliable computer-microcontroller communications over serial was challenging, as we did not initially realize that the two devices' IO operated at different voltages (3.3V vs 5V, respectively). Ensuring message validity (JSON formatting, delimiters, encoding, etc.) was also difficult, and was ultimately solved by writing unit testing suites for both devices' communication software. In addition, being able to flash microcontroller firmware from the computer on the fly required a number of revisions.

Cloud communication was also a major challenge, as registering an IoT device with an API required keypair generation. Manual/hard-coded keypairs are not user-friendly or particularly secure, and a cryptographic integrated circuit built into the motherboard goes against our open-source mission. We ultimately settled on the following process: the user securely enters their credentials for an authentication provider (Google, GitHub, Microsoft, etc.) using the OAuth Device Flow method, and the device "logs in" to our API as the user. The device can then request a private key from our API, which generates the keypair (supposing the user has not exceeded their device quota) and stores the public key on the backend. The device has effectively "self-registered", and can publish data and recieve live config data autonomously.

For a long time, the aeroponic container was a "black box" in our design, meaning that we had no idea where to even begin. One of our team members suggested tent material, and prototyping took off once we found the specific composite that met our requirements (high strength/low weight, fully waterproof, no coating/food-safe, relatively easy to assemble). Assembling the container required a number of iterations, as we initially overlooked the additional material needed for seams (fused with pressure-sensitive adhesive) when calculating the dimensions, as well as choosing a variant of the composite fabric that was too thin and ultimately developed microtears.

Logistical issues also presented themselves during the prototyping process. We had to move locations twice, and our primary 3D printer spontaneously broke.

Currently, the custom peristaltic dosage pumps are being prototyped. This requires tight tolerances in 3D printing, due to the specific mechanism of peristalsis (pump housing not tight enough, tubing does not fully shut under force from the rollers and water is able to backflow through the pump; pump housing is too tight, and the tubing wears out very quickly, or the rollers don't move at all, overheating the stepper motor). In addition, the thermoregulation systems (both leaf- and root-zone) are nearing completion, with only heat sink selection and subsystem testing remaining.

## Timeline

The timeline as of May 31st 2022 is as follows:
-   *June 2022*: Finish design. Continue prototyping. Grow our first plants and collect data.
-   *July 2022*: Complete prototype. Assemble and distribute prototypes to schools and volunteers for beta testing, publicly sharing collected data and diagnostic information. Make design/prototype improvements based on beta testing. Begin constructing the optimization machine learning model.
-   *August 2022*: Collect samples for nutritional and safety analysis. Collaborate globally on distributed phenological research. Publish findings.

Estimated latest prototype completion date: **August 1st, 2022**.