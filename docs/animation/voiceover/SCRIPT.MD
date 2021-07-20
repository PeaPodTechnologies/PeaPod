# Animation Voiceover Script

> Leverage visual medium to SHOW SETUP
>
> Leverage auditory medium to EXPLAIN PURPOSE, METHOD

- [Animation Voiceover Script](#animation-voiceover-script)
  - [Intro](#intro)
  - [Overview](#overview)
    - [Housing & Control Module](#housing--control-module)
    - [Extendability](#extendability)
  - [Subsystems: Setup and Description](#subsystems-setup-and-description)
    - [Lighting](#lighting)
    - [Grow Platforms](#grow-platforms)
    - [Tray Modularity](#tray-modularity)
    - [Aeroponics](#aeroponics)
    - [Humidification](#humidification)
    - [Dehumidification](#dehumidification)
  - [Maintenance](#maintenance)
  - [Outputs: Harvesting, Preparation, and Seed Harvesting](#outputs-harvesting-preparation-and-seed-harvesting)
  - [Optimization](#optimization)
  - [Conclusion](#conclusion)

## Intro

PeaPod is a precision-automated plant growth environment, designed as both a low-maintenance food production system and a distributed research tool. 

PeaPod is able to generate any desired environment, while collecting data on plant growth and improving yields over time.

## Overview

### Housing & Control Module

The growth environment features a frame-and-panel construction. 

Aluminum extrusion with engineering plastic brackets provide a lightweight and sturdy frame and include channels for easy component mounting and repositioning. 

Mylar-laminated foam panels provide a high degree of thermal insulation and reflect light internally, making PeaPod more efficient.

This approach to construction makes assembly quick and easy while also maximizing space efficiency during stowage.

Automation and control is made possible by PeaPod's control module, installed on top of the frame and connected to water, power, and network inputs.

### Extendability

PeaPod can accommodate different plant sizes and dimensional constraints by connecting frame units together and removing dividing panels. Larger configurations require additional control modules to extend the power of the primary unit. 

This extendable topology can be modified on the fly to suit the plant selection or mission requirements.

## Subsystems: Setup and Description

### Lighting

<!-- Description -->

Powerful LED boards provide light, featuring power modulation for precise spectrum control ranging from near-ultraviolet to near-infrared.

<!-- Setup -->

The boards are mounted to a removable subframe, daisy-chained and connected connected to the control module for power and control.

### Grow Platforms

<!-- Description and Setup (mostly visual) -->

Plants are grown in grow cups supported by a removable subframe, with aeroponic nozzles and root zone container and underneath.

### Tray Modularity

Both lighting and grow subframes are modular, enabling the user to add, remove, or reposition trays at will, creating new configurations with different root and leaf zone heights. 

### Aeroponics

PeaPod uses aeroponics for increased water efficiency, supported crop variety, and control over parameters such as pH and plant nutrient concentrations.

Reverse osmosis water is supplied to the control module where it is pressurized by a diaphragm pump and pressure tank. A pressure sensor sends data to the control module.

Water is heated and cooled by a thermoelectric water block. A temperature sensor informs the control module's PID process.

Water then flows through an injection manifold where nutrient and pH solutions are added.

The injection manifold comprises a set of parallel Venturi siphons, whose inlets connect to solution containers. Servo-actuated valves formulate precise nutrient dosage ratios and pH levels, and check valves prevent backflow.

A solenoid valve allows the control module to enable the flow of water to the nozzles on demand.

Each grow platform's nozzles are chained, and are mounted under the grow platforms in the root zone. Each grow platform's supply line is quick-disconnect, enabling the platforms to be removed or repositioned on demand.

### Humidification

Humidity is increased in the system by an ultrasonic mesh nebulizer. Reverse osmosis water is distributed to a small container, housing the mesh disc. When power is applied to the driver circuit, the mesh oscillates, producing a fine vapour at a fixed rate. Vapour is distributed throughout the environment by circulation fans.

Mesh nebulizers require no cleaning when using pure water.

### Dehumidification

Humidity is decreased in the system via silica gel beads. Fans pass air through a removable cartridge, housing the beads. Shutters on the opening allow for precisely timed automated control.

Silica gel is efficient, non-toxic, and changes color to indicate saturation. This is detected by the control module, which then notifies the user to removed and "recharged" the cartridge via evaporation, which can be accelerated by an oven.


## Maintenance

Maintenance requirements include: 

1. Daily manual pollination for flowering crops;
2. Occasional nozzle cleaning to prevent calcification;
3. Dehumidification cartridge recharging;

PeaPods will notify users when any maintenance is required.

## Outputs: Harvesting, Preparation, and Seed Harvesting

Harvesting varies by crop type. When crops are harvested, the yield can be stored in sealable containers and refrigerated for future consumption. 

Preparation method also varies by food type.

Seeds can be collected and stored to be planted later, creating a sustainable crop cycle. Alternatively, offshoots of certain plants can be harvested to "clone" the plant, skipping the germination step and accelerating growth.

## Optimization

Over the course of the growth cycle, PeaPod collects data on all environment parameters, as well as metrics of plant growth and health. This data is used to train a machine learning "surrogate model" - a functional representation of the plant's phenology across time. This model aims to fit a function to match the the plant-environment relationship, which can then be optimized for yield, nutrient concentration, flavour, energy efficiency, water use, or any other metric.

Plants grown in PeaPod will be more nutritious, taste better, and yield more as more iterations are performed.

## Conclusion

PeaPod provides unrivalled versatility and reliability to food production systems for long duration space missions.