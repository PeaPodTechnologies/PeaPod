# PeaPod

[![forthebadge](https://forthebadge.com/images/badges/made-with-crayons.svg)](https://forthebadge.com) [![forthebadge](https://forthebadge.com/images/badges/gluten-free.svg)](https://forthebadge.com) [![forthebadge](https://forthebadge.com/images/badges/open-source.svg)](https://forthebadge.com) [![forthebadge](https://forthebadge.com/images/badges/powered-by-coffee.svg)](https://forthebadge.com)

A [cloud-enabled](https://github.com/openformtech/cloudponics-app/) IoT automated plant growth environment. Designed as a research tool for precise and distributed mapping of the plant-environment relationship.

# Background

## What is a plant growth environment?

In the most relevant terms, the "plant growth environment" is used to simultaneously describe *a)* the collection of "`environment variables`" that determine plant growth (watering and amount of sunlight come to mind) and/or *b)* the physical `enclosure`, materials, methods, or otherwise that impose the former with the specific goal of plant growth - this can be anything from a farmer's field to a greenhouse.

<!-- ### (Intro to Hydroponics) -->

## How is this automated? What is the value derived from automation?

The key determining factors of agricultural efficacy are climate, land fertility, and agricultural practices. "Natural" agriculture - that which is performed in open fields, with direct exposure to the elements - is entire at the mercy of the climate and soil fertility. Most crops in North America and across the world can only be grown a fraction of the year due to regular climate fluctuations (aka seasons)\[[1](#References)\], and limitations due to soil conditions mean that crops have to be rotated\[[2](#References)\]. Economic losses from natural disasters and changing climate alone have reached a staggering average of 250-300 billion USD a year\[[3](#References)\]. 

Environment control and automation present a clear solution - if we could isolate the plant growth environment from natural pressures, an "`artificial`" approach to agriculture where the various factors are placed under human control and computerized, we could eliminate the inefficiencies and risks posed by natural factors.

### Greenhouses and Hydroponics

The most typical use case of this kind of technology is in greenhouses, where varying degrees of control (typically limited to watering, lighting, and temperature control) often in conjunction with `hydroponic` techniques enable numerous advancements: 
1. Being able to grow crops **year-round**;
2. Using **less water** (especially in the case of `hydroponics`, which can use up to 90% less water and produce more than 10 times higher yields\[[4](#References)\]);
3. Being able to grow crops in less "naturally" arable regions of the world (i.e. deserts, tundra, arctic/antarctic regions);

These advancements come at the sacrifice of accessibility - the greater the degree of automation and/or control, the higher the energy consumption and the more expensive the technology is to implement (especially in remote areas)\[[4](#References)\].

## What is the "plant-environment relationship"? 

TODO

## Why is mapping this important?

TODO

<!-- Treating plants like a functional unit - map of "input vector" environment to "output vector" plant growth and expression. 

Applying repeatability - the basis of scientific research - to plant cultivation. Full control over all independent variables. -->

<!-- ## Features 

### Data Collection

Precision, realtime, ...

- Variables

### Precise Environment Control

Each **PeaPod** device is an `isolated` growth environment, capable of generating a spectrum of naturally ocurring - or artificially designed - environments. Everything is under your control:

- Light spectrum and intensity
- Leaf- and root-zone temperatures
- Watering rate and schedule
- Nutrient dosage and pH

This fine degree of control allows for deep insight into what ***exactly*** makes plants tick.

### Data Collection

Data collected is scientifically meaningful.

### Instantaneous Data Collection -->

# Development

## Repository Structure

Software for the host computer (i.e. RPi) is located within `./computer/`. Software for the target Arduino is located within `./computer/arduino/`, so that when the computer code is downloaded, the Arduino code can be deployed from the host computer.

## Minimum Viable Product

One working PeaPod - performs all `necessary` functions:

- Can grow a plant
  - Can control lights, air temperature, watering cycle, nutrients and pH, to within [design requirements](./docs/design/DesignBrief.md)
  - Can run for a full plant lifecycle reliably with only operator attention
- Can collect data
  - Can collect data on air temperature, humidity, air quality, plant mass, and plant growth (camera) to within [design requirements](./docs/design/DesignBrief.md)
  - Can connect to da Firebase cloud

## Todo

All components to within [design requirements](./docs/design/DesignBrief.md).

- [ ] Sensors + computer
  - [ ] Software architecture
    - [ ] POST
    - [ ] Sensor reading
    - [ ] Cloud auth/IoT registry
  - [ ] Hardware architecture
    - [ ] Choose computer (WiFi)
  - [ ] Write software
  - [ ] Assemble hardware
- [ ] Prototype watering system (fogponics, nutrient+pH dosage system)
  - [ ] Prototype fogponics circuit
    - [ ] Design circuit, justify
    - [ ] Draw schematic
    - [ ] BoM V1
    - [ ] Buy parts
    - [ ] Assemble and test (fog generation)
  - [ ] Design dosage system
    - [ ] Design stepper motor -> syringe control parts
    - [ ] Write dosage control software
    - [ ] Print, assemble, test (volume-step calibration)
  - [ ] Design water import system
    - [ ] Select tank (water bottle?)
    - [ ] Select+calibrate import method (pump?)
  - [ ] Design reservoir
    - [ ] Assemble subcomponents, test
    - [ ] How to handle waste water? Flush?
- [ ] Lighting system (LED)
  - [ ] Select lights
  - [ ] Design circuit, justify
  - [ ] Draw schematic
  - [ ] BoM V1
  - [ ] Buy parts (PCB?)
  - [ ] Assemble and power-test
  - [ ] Write and test lighting control software (spectrum)
- [ ] Temperature control system (Peltier)
  - [ ] Select peltier+heatsinking module + control circuitry
  - [ ] Write temperature control software (PID)
  - [ ] Assemble and test, calibrate PID
- [ ] Master Assembly
  - [ ] CAD **ALL** components, subsystems, everything
  - [ ] Design Enclosure (3D printed casing)
    - [ ] Select insulation (foam?)
    - [ ] Reflective surface on inner walls?
  - [ ] Mounting and fasteners
- [ ] Assembly


# References

1. https://swat.tamu.edu/media/90113/crops-typicalplanting-harvestingdates-by-states.pdf
2. https://www.sare.org/publications/crop-rotation-on-organic-farms/physical-and-biological-processes-in-crop-production/crop-rotation-effects-on-soil-fertility-and-plant-nutrition/
3. http://www.fao.org/3/a-i6486e.pdf
5. https://www.ncbi.nlm.nih.gov/pmc/articles/PMC4483736/

## Not Cited

- https://www.ncbi.nlm.nih.gov/pmc/articles/PMC7516583/