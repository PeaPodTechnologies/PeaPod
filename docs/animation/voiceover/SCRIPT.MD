# Animation Voiceover Script

> Leverage visual medium to SHOW SETUP
>
> Leverage auditory medium to EXPLAIN PURPOSE, METHOD

> **ADD ON SCREEN ICONS FOR PROCEDURES ETC...**

> Last rough cut was ~5:40 - needs to be a tiny bit faster.
> Script was condensed quite a bit so no need for the pace to be a lot faster.
> 
<!-- Lol -->

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

Automation and control is made possible by PeaPod's control module, installed on top of the frame and connected to water, power, and network inputs.

This approach to construction makes assembly quick and easy while also maximizing space efficiency during stowage.

### Extendability

PeaPod can accommodate different plant sizes and dimensional constraints by connecting frame units together and removing dividing panels. Larger configurations require additional control modules to extend the power of the primary unit. 

This extendable topology can be modified on the fly to suit the plant selection or mission requirements.

## Subsystems: Setup and Description

### Lighting

<!-- Description -->

Powerful LED boards provide light, featuring power modulation for precise spectrum control ranging from near-ultraviolet to near-infrared.

<!-- Setup -->

The boards are mounted to a removable subframe, daisy-chained and connected to the control module for power and control.

### Grow Platforms

<!-- Description and Setup (mostly visual) -->

Plants are grown in grow cups supported by a removable subframe, with aeroponic nozzles and root zone container underneath.

### Tray Modularity

Both lighting and grow subframes are modular, enabling the user to add, remove, or reposition trays at will, creating new configurations with different root and leaf zone heights. 

### Aeroponics

PeaPod uses aeroponics for increased water efficiency, supported crop variety, and control over parameters such as pH and plant nutrients.

Reverse osmosis water is supplied to the control module where it is pressurized by a diaphragm pump and pressure tank; its temperature is regulated by a thermoelectric water block. 

The water then flows through an injection manifold where nutrient and pH solutions are added.

The injection manifold comprises a set of parallel Venturi siphons whose inlets connect to solution containers via servo actuated valves.

These valves then formulate precise nutrient dosaged and pH levels.

A final solenoid valve allows the control module to activate the nozzles on demand.

The nozzles are mounted in the root zone and are connected to the supply line which features quick-disconnect; "CLICK" for easy removal or repositioning.

The nozzles are robust and only require occasional cleaning to prevent calcification.

### Humidification

Humidity is increased by an ultrasonic mesh nebulizer housed in a small container with reverse osmosis water supplied to it. When the control module activates a driver circuit, the mesh oscillates, producing a fine vapour that is distributed throughout the environment by circulation fans.

### Dehumidification

Humidity is decreased via silica gel beads held within a removable cartridge.

Fans pass air through the cartridge and shutters on the openings allow for precise automated control. 

Silica gel is efficient, non-toxic, and changes color to indicate saturation. This is detected by the control module, to notify the user to remove and "recharge" the cartridge via evaporation in a standard oven.


_## Maintenance??? is section even needed???

_Maintenance requirements include: 

_1. Daily manual pollination for flowering crops;
_2. Occasional nozzle cleaning to prevent calcification; (covered)
_3. Dehumidification cartridge recharging; (covered)

_PeaPods will notify users when any maintenance is required. (mentioned)


## Planting, Harvesting, Preparation, and Seed Harvesting

Harvesting varies by crop type but the yield can be stored in sealable containers and refrigerated for future consumption. 

The preparation method also varies by food type and preference but are all common processes.

Seeds can be collected by hand and stored to be planted later, creating a sustainable crop cycle. 

Alternatively, offshoots of certain plants can be harvested to "clone" the plant, skipping the germination step and accelerating growth.

Planting seeds and offshoots both involve a neoprene puck which is then inserted into the grow cups, PeaPod takes care of the rest. 

## Optimization

Throughout each growth cycle, PeaPod collects data on all environment parameters and plant metrics.

This data is then used to train a machine learning surrogate-model to represent the plant's phenology across time.

This model aims to fit a function to the plant-environment relationship, which can then be optimized for yield, nutrient concentration, flavour, energy efficiency, water use, or any other metric.

Therefor plants grown in PeaPod will be more nutritious, taste better, and yield more as more iterations are performed.

## Conclusion

PeaPod provides unrivalled versatility and reliability to food production systems for long duration space missions.
