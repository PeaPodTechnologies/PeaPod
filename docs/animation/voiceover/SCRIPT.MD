# Animation Voiceover Script

- [Animation Voiceover Script](#animation-voiceover-script)
  - [Intro](#intro)
  - [Overview](#overview)
    - [Housing](#housing)
    - [Control Module](#control-module)
    - [Extendability](#extendability)
  - [Subsystems and Features](#subsystems-and-features)
    - [Lighting](#lighting)
    - [Grow Platforms](#grow-platforms)
    - [Tray Modularity](#tray-modularity)
    - [Aeroponics](#aeroponics)
    - [Humidification and Dehumidification](#humidification-and-dehumidification)
  - [Maintenance](#maintenance)
  - [Harvesting, Preparation, and Seed Harvesting](#harvesting-preparation-and-seed-harvesting)
  - [Optimization](#optimization)
  - [Conclusion](#conclusion)

## Intro

PeaPod is a cloud-connected automated plant growth environment, designed as both a low-maintenance automated food production system and a distributed citizen-science research tool. 

PeaPod is able to generate any environment from a combination of independent environment parameters, enacted by a diverse set of precision actuators and reinforced by a complementary set of sensors.

## Overview

### Housing

Housing for the growth environment is provided by a frame-and-panel construction.

Aluminum extrusion and engineering plastic brackets provide a lightweight and sturdy frame with channels for easy mounting and repositioning of components.

Mylar-laminated foam panels provides a high degree of thermal insulation and light isolation, while remaining lightweight and increasing energy efficiency by reflecting light internally.

This approach to construction makes assembly quick and easy while also maximizing space efficiency during stowage.

### Control Module

Each PeaPod also includes a control module in which all mechanical and electronic control systems are located. These subsystems together are what allow PeaPod to induce precise environmental control.

Control modules are mounted externally on top of PeaPod frame units and are connected to water, electricity, and network inputs.

### Extendability

PeaPods can be extended by merging frames, allowing systems to adapt to different plant sizes or other dimensional constraints. 

Dividing panels may be removed, and the extended multi-frame PeaPod may either be operated from a single control module, or by many (depending on the size) in a controller-follower topology with one primary module controlling all other modules.

This multi-frame topology can be modified at any time to suit the plant selection.

## Subsystems and Features

### Lighting

Lighting is provided by custom high-power LED boards. 

These boards allow for precise spectrum control via power modulation (or 'dimming') on LEDs of a multitude of different wavelengths, ranging from near-ultraviolet to near-infrared.

The boards are daisy-chained together, and one is connected to the control module for power and modulation control of the entire set.

### Grow Platforms

Growth platforms are flat plane constructions with support for grow cups. These platforms support the plants, the root zone container, as well as the aeroponic nozzles.

### Tray Modularity

Grow platforms and lighting systems are both mounted to horizontal plane frames, or trays, that are position-adjustable and able to 'slide' into and out of the PeaPod housing at will. This makes both planting and harvesting processes far easier, as well as enabling the user to add, remove, or reposition trays at will, creating new environments with different root and leaf zone heights.

### Aeroponics

PeaPods use aeroponics for increased water efficiency and supported crop variety. This method also provides the greatest degree of control over parameters such as pH and nutrient availability.

Reverse osmosis filtered water is supplied to the control module under positive pressure, priming the diaphragm pump, where it is pressurized to 80 pounds-per-square-inch by the diaphragm pump and accumulator tank.

Water then flows through an injection manifold where it is mixed with nutrient and pH solutions in-line at an adjustable ratio before reaching the nozzles.

The injection manifold comprises a set of parallel Venturi siphons, with the inlets of each connecting to one of the solution containers. Servo-actuated needle valves on each inlet line formulate a precise dosage ratio, and check valves prevent backflow when the solenoid is closed. Float sensors in each solution container allow the computer to notify the user when to refill.

A normally-closed solenoid valve then allows the computer to control the flow of water to the nozzles.

The nozzles are chained via inline tees, and are mounted under the grow platforms in the root zone. Each grow platform's nozzle line is connected to the aeroponics supply by a quick-disconnect connector and a tee, enabling many grow trays to be connected and disconnected on demand. The nozzles require only occasional cleaning to prevent buildup from any mineral nutrients in solution.

### Humidification and Dehumidification

The head also contains the humidifier and dehumidifier.

Humidity is increased in the system by an ultrasonic mesh nebulizer. Reverse osmosis water is distributed to a small container, housing the mesh disc. When power is applied to the nebulizer driver circuit, the mesh oscillates, producing a fine vapour at a fixed rate. Vapour is distributed throughout the environment by circulation fans.
Mesh nebulizers are quick, and require no cleaning when using pure water.

Humidity is decreased in the system via silica gel beads. Fans pass air through a removable cartridge, housing the beads. Shutters on the opening allow for precisely timed automated control.
Silica gel is efficient, non-toxic, and change colors to indicate saturation. This is detected by the computer, which then notifies the user. The cartridge can then be removed and "recharged" via evaporation, which can be accelerated by an oven.

## Maintenance

Other simple maintenance requirements include: 

1. Daily manual pollination for flowering crops. 

PeaPods will notify users when any maintenance is required.

## Harvesting, Preparation, and Seed Harvesting

Harvesting is straightforward. Method varies based on crop type. When crops are harvested, the yield can be stored in sealable containers and refrigerated. 

Preparation method and time also varies between plants.

Like wise, seeds can be collected and stored to be planted later, creating a sustainable crop cycle. Alternatively, offshoots of certain plants can be harvested to "clone" the plant, skipping the germination step and accelerating growth.

## Optimization

Over the course of the growth cycle, PeaPod collects data on all environment parameters, as well as metrics of plant growth and health. This data are treated as inputs and outputs, respectively, to a "surrogate model" - a functional representation of the plant across time. This function is implemented as a machine learning model, which aims to fit a function to match the the plant-environment relationship, which can then be optimized for yield, nutrient concentration, flavour, energy efficiency, water use, or any other metric or compound metric.

Because of this, plants grown in PeaPod will be more nutritious, taste better, and yield more over time as more crops are grown.

## Conclusion

PeaPod provides unrivalled variety, versatility and reliability to food production systems for long duration space missions.