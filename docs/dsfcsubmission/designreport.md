# Design Report - Plaintext <!-- omit in toc -->

Table of Contents

- [Design Abstract](#design-abstract)
- [Description](#description)
  - [Part A](#part-a)
  - [Part B](#part-b)
- [Innovation](#innovation)
- [Adherence to Constraints](#adherence-to-constraints)
  - [Outer Dimensions, Volume](#outer-dimensions-volume)
  - [Power Consumption](#power-consumption)
  - [Water Consumption](#water-consumption)
  - [Mass](#mass)
  - [Data Connection](#data-connection)
  - [Crew Time Requirement](#crew-time-requirement)
  - [Operational Constraints](#operational-constraints)
- [Performance Criteria](#performance-criteria)
  - [Acceptability](#acceptability)
    - [Acceptability of Process](#acceptability-of-process)
    - [Acceptability of Food Products](#acceptability-of-food-products)
  - [Safety](#safety)
    - [Safety of Process](#safety-of-process)
    - [Safety of Food Products](#safety-of-food-products)
  - [Resource Inputs and Outputs](#resource-inputs-and-outputs)
    - [Resource Inputs](#resource-inputs)
    - [System Outputs](#system-outputs)
    - [Optimization](#optimization)
    - [Food Output Quality](#food-output-quality)
    - [Additional Comments](#additional-comments)
  - [Reliability and Stability](#reliability-and-stability)
    - [Process Reliability](#process-reliability)
    - [Input and Output Stability](#input-and-output-stability)
- [Terrestrial Potential](#terrestrial-potential)

# Design Abstract

<!-- 1499 chars -->

PeaPod is a modular, automated plant growth environment and distributed research tool. It can generate any environment for any crop, while collecting plant growth data for optimization.

The growth environment is adaptable to many plants and mission requirements. It consists of "unit cell" cubes that can be joined upwards and sideways to increase space. It is insulated with internally-reflective panels for efficiency [1].

PeaPod uses automated control systems to generate desired environments. These are air thermoregulation, humidity control, LED lighting, and an aeroponics system. They are automated by an onboard computer and housed in a "control module" at the top of the unit. This lets power be "multiplied" for extended PeaPods by adding more control modules in a controller-follower topology.

Plant growth cups and lighting platforms are built on modular "trays" mounted to the inside of the housing so the user can position plants and lights to accommodate any plant size.

Throughout a growth cycle, all environment parameters and growth metrics are collected. This data trains a statistical ML model to represent the plant’s phenology over time in the environment. The output function can then be optimized for yield mass, nutrient concentration, flavour, resource efficiency, or any other metric.

Combining these into an accessible open-source design, PeaPod provides unrivalled versatility and reliability to food production systems both terrestrially and on long-duration space missions.

# Description

## Part A

<!-- 2863chars -->

PeaPod is an automated plant growth environment, made of control systems and an automation/monitoring system within a modular, cubic housing. It can generate any desired environment while collecting data on plant growth and improving yields.

NOTE: For this submission, only the 4x3 unit "Standard Extended PeaPod" is in consideration. It has one control module and one of each tray (grow and lighting) per unit.

PeaPod's control systems are made of environmental controls (feedback loops with sensors) and plant inputs (set-states):

Lighting: LEDs, from near-ultraviolet to near-infrared. Dimmable drivers for precision spectrum and intensity control. Efficient, precise emission spectrum, low heat.

Aeroponics: Reverse osmosis (RO) water is pressurized by a pump (with sensor for safety cutoff), brought to temperature, nutrient-dosed and pH-balanced by Venturi siphons with servo-actuated flow control, and forced through nozzles to generate mist. Runoff water is recycled. Water-efficient (98% less than farming), nutrient-efficient (60% less), no pH/nutrient "feedback" (common in reservoir-based hydroponics), increased root oxygenation [2].

Air Thermoregulation: Leaf zone air temperature is regulated by a thermoelectric heat pump. Fans blow air over heat sinks connected to either face of a Peltier tile to circulate air and dissipate heat. A Proportionate-Integral-Derivative (PID) control system is informed by temperature sensors, and controls the direction and magnitude of the heat transfer. Low complexity, high safety/reliability, easy to automate (bidirectional, precisely dimmable, PID tuning).

Humidity Regulation: Leaf zone humidity is regulated by a dead-zone bang-bang control system informed by humidity sensors.

Humidification: RO water is supplied to a tank with a fine mesh piezoelectric disc. A controllable driver circuit oscillates the disk, producing water vapour. Easy to automate.

Dehumidification: A dry silica gel bead cartridge is covered by servo-actuated "shutters" to control dehumidification. Fans draw humid air through a HEPA filter into the desiccant and back into the growth environment. The beads change color to indicate water saturation. The crew is then notified to swap and "recharge" in a standard oven.

Aeroponic Water Temperature: Root zone air temperature is regulated in the same way as the leaf zone system. Exceptions include an aluminum water block (vs internal heat sink and fan) and a single temperature sensor after the block for PID feedback in a flowing system.

Gas Composition: Oxygen and carbon dioxide levels are managed by gas exchange. Input and output ports allow fans to draw air into and out of the system. HEPA filters remove microbes and aerosols, and servo-actuated "shutters" prevent unintended exchange. Gas concentration sensors inform a bang-bang control system for port activation.

## Part B

<!-- 1452 chars -->

Once PeaPod has been assembled, the first step is manual sterilization to prevent microbial growth at high growth environment humidities. Any plant- and food-safe means may be employed (i.e. UV).

Next is the hookup of power, network, and reverse osmosis water inputs, as well as the filling of nutrient and pH adjustment solution containers.

PeaPod's operation begins at planting. Seeds are removed from vacuum-sealed storage bags and placed in neoprene foam "pucks", which are moistened for germination and placed in grow cups. The front panel is then closed, sealing the internal environment.

The desired state of the growth environment is encoded via a standardized set of "environment parameters". The specific values of each parameter for each iteration are stored in a "program". The program is set prior to the start of plant growth, and consists of a set of actions (e.g. set red LEDs to 63% power) and control targets (e.g. hold air temperature at 22°C) as time-series instructions from the start of planting to the point of harvest (or plant death, in the case of multiple-harvest plants) corresponding to the control systems.

When activated, PeaPod's control systems begin to enact the program. PeaPod will notify crew of any required action, including refilling inputs, cleaning components, and harvesting, as well as "End-of-Program", when all inedible plant matter is disposed of. The process can then be repeated (starting at sterilization).

# Innovation

<!-- 2810 chars -->

Control Range and Parameter Independence

Unlike large-scale vertical farming, PeaPod’s isolation and parameter independence lets it simulate any climate. Wide LED spectrum can emit both near-infrared and near-ultraviolet light, important for creating hormonal responses and compounds in plants. Combined with insulation, humidification and dehumidification, and thermoelectric heating/cooling, PeaPod can generate extreme environments and even conditions on other planets (minus gravity and atmospheric changes). 

These parameters are independent: e.g., lighting heat is countered by thermoregulation cooling. Also, inline nutrient and pH solution injection eliminates drawbacks of reservoir use by taking less space, less solution mixing time, and avoiding a control loop of nutrients/pH (while also being more accurate). 


Form Factor and Extendability

The range of output environments is also possible because of the form factor of a single PeaPod unit. Warehouse-scale vertical farming cannot provide wide-spectrum control due to size---poor insulation and air circulation prohibits extreme temperatures. PeaPod solves this with a small, modular design that enables robust lighting, heating, and cooling systems at home and at scale.

Space savings are a benefit of this feature, as the output of an entire farming or hydroponics setup (requiring a flat field or warehouse) can be spread through unused space (corners, under shelves/desks, etc.) via many small PeaPod setups. This means a large yield can be had without construction, zoning, labour, or any of the other issues accompanying large farming or hydroponic setups.

Optimization

Existing approaches to plant optimization are simple and ineffective, relying on a fixed/unchanging environment parameter set and only examining final plant metrics. This approach is severely limited, in that it does not account for changes over time.

Instead, statistical model is used which takes into account the cumulative property of growth. By monitoring all environment and plant indicators, repeatable and controlled trials with scientific validity are able to be performed. PeaPod counters declining health/quality indicators in real time, and generates tailored programs that to maximize any metric and target further improvement.

See the preliminary calculations Appendix A.5 for details.


Open-Source Design and Data

Since PeaPod is standard and open-source, units can be had in bulk and assembled by anybody. Public contribution to the project (both in design improvement and data) increases the reliability and safety of the solution. Collected data is, ideally, committed to the public, meaning anybody with a PeaPod can run the same iteration with the same program and species to boost scientific validity, or run a different program to expand PeaPod's knowledge base.

# Adherence to Constraints

## Outer Dimensions, Volume

Standard Expanded PeaPod: 4x3x1 units (0.5m on all sides) + control module = 2m wide x 1.7m tall x .5m deep = 1.5m^3 (< 2m^3)

With width treated as depth for the purposes of the considerations of the "room size" constraint, the Standard meets this constraint.

## Power Consumption

For calculations and justification, see preliminary calculations Appendix A.1.

Total Power Consumption: 1,284W (< 1500W)

## Water Consumption

For calculations and justification, see preliminary calculations Appendix A.2.

Humidification: CNet = 500mL initial + 50mL per hour

Aeroponics: CNet = 1.25L primed + 1.2L per hour

## Mass

For part breakdown, calculations, and justification, see preliminary calculations Appendix A.3.

Total: 70kg

## Data Connection

Remote Control: The program may be changed instantaneously as an appended instruction set with immediate effect.

Data Presentation: Plant and environment data can be viewed with live updates and video feed.

## Crew Time Requirement

For calculations and justification, see preliminary calculations Appendix A.4.

Total setup process time (Standard 2 trays per unit, 12 units, 1 CM): 17.5 hours (one person) or 4.5 hours (crew of 4)

Total maintenance time per week: 1-2 hours (depending on program)

## Operational Constraints

Design operates in terrestrial conditions.

Ambient pressure: tank, bladder, and nozzle are designed to produce indicated outputs at standard air pressure

Ambient temperature and humidity: less concern, housing is sealed and insulated

# Performance Criteria

## Acceptability

### Acceptability of Process

<!-- 2767 chars -->

Footprint:

Due to PeaPod's modular construction, the footprint can vary. For a Standard Extended PeaPod, the footprint would be 2m x 0.5m, or about three standing refrigerators. When stowed, volume is reduced to 37% of when assembled. Control module is packed pre-assembled.


The following estimates are for a Standard Extended PeaPod.

Setup Process:

Assemble housing - 2 hours
Install control module(s):
  Hook up water, power, and network inputs - 5 min
  Fill nutrient and pH adjustment solution containers - 10 min
  Mount CM to housing - 5 min

Assemble all trays - 1 hour

For each tray, either:
  Mount lighting boards and driver, daisy chain boards to driver, hook up power and signal to driver and CM - 10 min per tray, OR
  Mount aeroponic nozzle mount and arm, hook up water delivery line to nozzles and CM - 10 min per tray

UV sterilization - 20 min

Prepare and plant seeds for desired crop output, seal growth environment - 30 min

Enable primary power supply, and power on automation system, allow to perform self-test and calibrations - 10 min

Open water input shutoff valve

Input program for required environments and activate - 5 min

Total setup process time (Standard: 2 trays per unit, 12 units, 1 CM): 8.5 hours (one person) or 2.5 hours (crew of 4)


Food Production Cycle:

Environment is maintained, and environment can be observed live at a computer terminal via sensor data and camera feed

Circulation fans enable automated pollination

Perform maintenance, including:
  Cleaning nozzle once a month - 10 min
  Swapping and recharging dehumidification cartridges when instructed - 5 min (active time)
  Refilling solution containers when instructed - 5 min

Upon End-Of-Program (EOP) notification, the gas exchange system will conduct a "full equalization flush", bringing the internal environment in equilibrium with the surroundings. Users will harvest and store food products (or prepare and consume them immediately, varying time) - 15 min

Upon End-Of-Life notification (may occur at the same time as EOP), the plant is scrapped (15 min), and new plants may be planted

Total maintenance time per week: 1-2 hours (depending on program)


Process Evaluation:

Setup and maintenance processes are fully documented in a "User Manual", which includes both text instructions (with numerical specifications for different actions) as well as diagrams for reference. Notifications from computer refer users to specific subsections of the Manual for maintenance actions. All processes require no specific expertise, just the ability to operate basic hand tools and follow instructions.

All interactions with the automation system (i.e. program upload, environment/camera monitoring) can be accomplished either via a touchscreen panel on the front of the control module or over the Internet.

### Acceptability of Food Products

<!-- 1159 chars -->

There are several considerations when examining the acceptability of the products of our system:

Produce is not only eaten fresh, but also forms the basis for an innumerable variety of combined and prepared foods (i.e. fresh tomatoes vs. tomato sauce).

When considering prepared derivatives of the food products, the quality of the preparation is a key factor in acceptability. As such, proper care in training and is to be taken.

The products formed by the system (and their properly prepared derivatives) are not exceptional or novel. They are the same plant-based foods grown, consumed, and accepted terrestrially, just grown in a more efficient and controlled way. As such, their acceptability is determined to be of equal or greater value.

Plant-environment optimization can be targeted not only at nutritional value or efficiency, but also at acceptability. The feedback can be gathered either through crew Hedonic rating (i.e. tomatoes grown in environment ABC rate X in appearance, Y in aroma, etc.) or more sophisticated analysis (i.e. computer vision analysis of color/size/shape for appearance, tissue concentrations of various aroma/flavor compounds).

Hydroponic crops have seen commercial success [3], suggesting that their output is of sufficient hedonic quality to be desired.

Case Study in Fresh Produce: Acceptability of Fresh Cantaloupe Melon [4]:
Appearance: 7.93/9.00
Aroma: 7.77/9.00
Flavor: 6.83/9.00
Texture: 7.43/9.00
Overall: 7.17/9.00 (>6.00)

## Safety

### Safety of Process

Being a sustainable isolated unit, PeaPod requires little cleaning, limited to UV sterilization each time it is opened.

PeaPod uses safe materials in its chassis, insulation and circuitry. 
The main frame is constructed using aluminum. Although large quantities of aluminum in food are deemed dangerous, the lack of direct contact exposure of aluminum to the plants passes well below the toxicity limit [5].
The bracketing and mounts of PeaPod are constructed using PETG plastic which has been deemed “food-safe plastic” [6].

The foam insulation used in PeaPod is commonly used for housing and other common safe applications. 

To avoid toxins in circuitry, lead-free soldering was used for all electronics. The dehumidification system uses silica gel, which is commonly found in food packets and is deemed “biodegradable and non-toxic” [7].

All voltages are sub 48V DC, avoiding any high-voltage risks. The voltage risk is also mitigated by short-circuit/over-current protection. 
All pressures experienced by the aeroponics system stay below 100 PSI, avoiding dangers with high pressures. The dangers with pressures are also mitigated through the use of PTFE tape, fail-safe solenoids (which primarily stay closed) and a pressure sensor shutoff. 

Materials selection eliminates the risk of off-gassing. The presence of microbes or other harmful pathogens are mitigated through the use of clean seeds, reverse osmosis water and pure nutrient/pH solutions, as well as "gas exchange equilibrium" and UV sterilization pre- and post-opening. HEPA filters on both gas exchange ports and the dehumidification cartridge (which is removed from the chamber) mitigate microbial and aerosol presence. Through a nutrient injection manifold, PeaPod also has the ability to administer custom solutions, such as anti-pathogenic compounds (fungicides, algicides).
In addition, PeaPod provides plant nutrients directly without the use of nitrogen-fixing bacteria. The production process is fully automated, mitigating all risks associated with human error. 

In the event of a malfunction, PeaPod also allows the user to override the program for the purposes of editing or shutting down the unit.

### Safety of Food Products

PeaPod provides astronauts the ability to select and grow produce of their choosing. Before takeoff, the representatives responsible for selecting and scheduling food resources for the duration of the trip must create a stockpile of seeds that will provide ample food for the astronauts throughout their journey. The variety, quality, and acceptability of crop selection is the primary variable for repeated consumption.

By maintaining optimal growth cycles, PeaPod ensures that the food produced is clean, varied, and fresh. However, program selection (especially those with chemical components, i.e. nutrient and pH solutions) also play a role, as these directly influence the composition of the food products.

The selection of proper crops and solutions, along with proper harvesting and processing techniques (i.e. only harvesting edible bodies, cooking for long enough) and harvesting/shelf-life timing (mitigated by notification), are the only concerns when it comes to product safety.

## Resource Inputs and Outputs

### Resource Inputs

Infrastructural Inputs: Reverse osmosis water (constant supply at positive pressure from onboard RODI system), nutrient solutions (stored, one container each plus refill tanks), pH solutions (one container pH up, one container pH down, plus refill tanks, stored), power (onboard power, standard 120V AC 60Hz), network connection (onboard network, for remote control, live video/data transmission), plant seeds (stored in vacuum-sealed seed bank, selected for variety and acceptability), input air (HEPA filtered, carbon dioxide-rich)

Process Inputs: Plant species identifiers, environment program (for entire growth cycle, one per plant species), nutrient and pH-adjustment solution identifiers (compounds and molarities, i.e. solution A is 0.6M NaNO_3)

Common nutrient solutions target specific ions, including bioavailable nonmetals (nitrates/nitrites, ammonia/ammonium, phosphates, sulphates), metals (potassium, calcium, magnesium, iron) and other trace elements.

### System Outputs

Products: Edible plant matter, recorded environment data, plant metric data, live video feed, time-lapse capture

Byproducts: Inedible plant matter (stems/roots/leaves/etc., waste), sensible heat (from thermoregulation pumping, managed by onboard heating/cooling), exhaust air (via HEPA filter, sterilized and dehumidified by onboard life support, oxygen-rich), minimal water vapour (as a result of higher air humidity, minimized by housing seal), latent heat (as a result of higher leaf zone temperature, minimized by insulation)

### Optimization

<!-- 1494 chars -->

High Success Rates: Complete automation and environmental control ensures high crop success rates and yield predictability.

Repeatability: Once optimal conditions are found for a given crop species, they can be repeated ad infinitum.

Immediate Sensor Feedback and Response: Immediate feedback from both environment sensors and plant metric analysis empowers the system to respond to unpredictable or otherwise uncontrolled factors (i.e. poor seed health, outside interference). Plant metric analysis can be used to diagnose program ineffectualities, accelerate optimization, and mitigate plant health decline.

Data Collection, Yield Optimization: By collecting data via computer vision and post-harvest yield evaluation (GCMS, weighing, etc.) on the plant's response to the induced environment, the relationship between the species behaviour and the surrounding environment can be analyzed. Plant metrics include plant health indicators (chlorophyll concentrations/chlorosis, leaf count/size distribution/density, plant height/canopy dimensions leaf tip burn, leaf curl, wilting, etc.) and crop yield (edible matter net mass/percent mass of plant, total plant mass, chemical/nutritional composition, caloric measurement, etc.). Data is filtered/smoothed across time to account for noise. The relationship is then represented by a statistical/machine learning model via a method known as "surrogate modelling". The method for this analysis can be found in the preliminary calculations Appendix A.5.

### Food Output Quality

<!-- 2545 chars -->

Given the system can induce a wide and continuous range of environments, it can produce environments suitable for any aeroponically-growable crop. Within the 2 square meters allotted to the solution, 12 PeaPods can be placed in a Standard configuration, resulting in a hypothetical maximum of 4 different environments (4 vertical columns with 4 control modules). The sum of the plants grown can be any combination of any number of suitable plant species (grouped into the same environment if suitable, i.e. different microgreens together), and as such, can be selected to meet all macro and micro nutrient requirements (with fortification or supplementation of those nutrients not found in plants [8]).

For example, quinoa - a crop already highly dense in nutrients (protein values up to 12-18% of mass, unique amino acid composition high in lysine) - has shown excellent potential for hydroponic/aeroponic growth in controlled environments with increases in nutrient density and yield (up to 37% harvest index, aka edible yield as a percentage of total mass). "Initial results indicate that quinoa could be an excellent crop for [controlled-environment agriculture] because of high concentration of protein ... and potential for greatly increased yields in controlled environments." [9] 

NOTE: Despite promising results, the experiment cited was performed with "no attempt to maximize productivity" [9]. When combined with the optimization routine, yields could be maximized even further.

Other crops suitable to aeroponics are listed here alongside their benefits and some examples of nutrient analysis:

Microgreens (sunflower sprouts, beansprouts, etc.) - Fast growth, edibility raw (minimal processing), more concentrated nutrients (9-40x higher than mature greens [10]). High in a variety of vitamins and minerals [11].

Legumes (soybeans, chickpeas, etc.) - High caloric density. For 100g boiled soybeans: 173 Calories. High in protein (16.6g), carbohydrates (9.9g) which are mostly fiber (6.0g), polyunsaturated fats (5.1g) and Omega-6 fatty acids (4.5g) [12].

Leafy Greens (lettuce, spinach, cabbage, kale, etc.) - Fast growth (more bulk output, more filling), edibility raw (minimal processing), versatility. For 100g raw spinach: 23 Calories. Contains protein (2.9g) and carbs (3.6g) which are mostly fiber (2.2g), as well as a variety of vitamins (A, C, K1, folic acid) and minerals (iron, calcium) [13]. 

These, in addition to herbs, berries, grains, garden produce, and root vegetables meet the vast majority of crew nutritional requirements.

### Additional Comments

Let it be noted that the primary goal of this design is not to satisfy the nutrient or caloric constraints. It is of the opinion of the submission team after extensive study that there is no way to produce 10,000 Calories in a 2 cubic meter environment via crop growth. The closest we got was a method for the production of aeroponic minituber potatoes as described in [14], which produces an estimated 2,000 Calories per day in a 2 cubic meter space.

Instead, this system caters to the more "human" aspects of food - palatability and enjoyability, versatility of products for different cuisine, diversity of outputs, and the positive effects of growing plants on human emotional health, to name a few.

## Reliability and Stability

### Process Reliability

<!-- 1341 chars -->

By nature of its design, PeaPod will at least last three years at near 100% functionality on minimal maintenance. This is achieved by self-monitoring component health, using easily-serviceable components, and providing smart notifications to the user when maintenance is needed.

For one, PeaPod is designed to be assembled by a single user with readily available hand tools. This means it can be disassembled, cleaned, and put back together by one person in a non-restrictive amount of time.

For another, the sensors used to monitor plant health and environment conditions allow PeaPod to perform self-diagnostics and notify the user when a part needs to be fixed or replaced. For example, if humidity readings fall despite power being applied to the humidification system, PeaPod will notify the user to check the humidification unit. If temperature readings fall despite power being applied to the thermoregulation system, PeaPod will notify the user to check the heating unit.

This said, every component in PeaPod has an expected lifespan over three years. From the LEDs (rated for 5 years) to the nozzles and fittings (high-quality brass), replacement monitoring is only needed as a backup. A replacement for each "active" part used in the entire assembly (i.e. non-housing, all moving/electrical/water parts) should be kept on board.

There are few moving parts, and no wear or lubrication required. The diaphragm pump is the most reliable and long-lasting pump variety [15]. Fans are self-lubricating nylon. Solenoids and servos are rated for upwards of 5 years.

### Input and Output Stability

<!-- 1171 chars -->

PeaPod's input stability is maximized by a variety of design choices, the sum of which give them a shelf life above the three-year mark of a mission. Since the system doses nutrients automatically and at a high-degree of precision, nutrient solution can be stored at a much greater density than would be possible with manual mixing. This minimizes degradation and loss of quality while reducing the space needed to store the solutions. Solutions are also stored in insulated, opaque container that minimize fluctuations in temperature and light that could stimulate compound degradation.

Outputs will have a shelf life that is, in worst case, comparable to fresh produce grown outdoors. More realistically, crops are expected to last longer as a result of a lack of pests, disease, and optimization of plant characteristics for ambient conditions, as well as eliminating time and transport between cultivation and consumption. Finally, PeaPod can let users grow crops on a rotation, providing a steady supply of fresh produce that will not need to be stored for particularly long periods of time, thus circumventing some of the restrictions posed by growing fresh crops.

# Terrestrial Potential

Agriculture-as-a-Service

PeaPod’s modularity and ease of storage can turn unused city spaces to PeaPod farms. With fresh produce in local areas, PeaPod can be a direct food system to paying customers. A subscription service would provide patrons with fresh produce without a middle man. By eliminating transport, distribution, and grocery stores, PeaPod creates fresher, better produce for the general public at a lower cost.


Crowd-Sourced Research

PeaPod's automation is unique in the research space, allowing for autonomous, off-site research. Universities save costs related to space and energy usage by subsidizing PeaPods to individuals, schools, or even restaurants. Users receive sets of parameters to grow crops with, sending data back to the institution and using the produce at the cost of space and energy. The result is a massive dataset from identical conditions in different places, verified by comparison with devices conducting the same tests.

This is an effective tool in climate change famine aid. By predicting conditions in at-risk areas, researchers can conduct tests ahead of time to determine what seeds, traits, and care parameters are most effective for certain conditions. This also informs development of seeds specialized for extreme climates, letting areas counteract food scarcity by having a variety of options prepared ahead of time.


De-centralized Production

Many crops are endemic to certain climates, making global transport necessary to for foreign markets. This reduces freshness, necessitates preservatives, and increases the carbon footprint of agriculture. By upscaling PeaPod technology to a farm scale, climate-bound crops can be produced anywhere.

PeaPod's form factor makes it a viable tool for at-home production, either in cities or off-grid. With only a solar power source, water, and a compact supply of nutrient solutions, users can sustain crops even through winter without travelling for nutrients and supplies.


Food Infrastructure Micro-Loans

For many, finding fresh produce is a struggle whilst growing your own is prohibitively expensive [16]. Micro-loan platforms have attempted to solve this by letting donors fund an interest-free loan for technology/infrastructure which then pays the loan as a percentage of its surplus. 

Unfortunately, these are only feasible for individuals in rural areas with arable land and climate.

PeaPod brings this solution to low-income urban areas with a platform for donors to micro-loan PeaPod units and inputs. The user feeds themselves and sells surplus, while a percentage of sales go to the interest-free loan. Once paid, PeaPod continues to produce food while sales fund its operation. 

This creates permanent, self-sustaining agricultural infrastructure that pays for itself as it grows, requiring little initial capital. This means entire farms throughout high density buildings generating yield with little lost space and almost no labour.