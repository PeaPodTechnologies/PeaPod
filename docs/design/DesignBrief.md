# Design Brief

# The Opportunity

> A botanical research tool for evaluating the relationship between a plant’s surrounding environmental conditions and its phenomic expression.

## Design Requirements

> What specific functions **must** candidate designs have in order to address the problem as it stands?

1. Internal environment can be `controlled` precisely and accurately, with specific focus on facilitating plant growth;
2. Accurate and precise `environment` data collection;
3. Accurate and precise (non-invasive) `plant` data collection.

## Stakeholders

> Whose wants or needs are we considering in the formulation of candidate designs?

1. Hobbyist consumer
2. Researcher
3. Enterprises
4. Schools
5. Standard consumer

# Objectives

> What do our stakeholders **value** in a solution, alongside/about necessary functions? What should be valued in a candidate design?

## High-Level

1. DIY/"`Maker`" friendly (S1, S4)
2. Option for `pre-assembled` units or kits (S1, S2, S3, S4, S5)
3. Redundancy, durability, modularity, replaceability (S1, S2, S3, S4)
4. Collects scientifically `meaningful` data on plant growth and environment (**R2**, **R3**, S2, S3, S4)
5. Controls the `meaningful` components of the plant environment (**R1**, S1, S2, S3, S4)
6. Internal environment is `isolated` from external environment (**R1, R2, R3**)
7. Environment actuation is `self-homeostatic`, meaning that if you turn the lights on, the environment *doesn't* warm up (**R1, R2, R3**)
8. Connectivity and `fleet` deployability (S2, S3)
9. Ease of use (S1, S4, S5)
10. `Aesthetics`, visual appeal (S1, S4, S5)

## Low-Level

1. DIY has "light fabrication" `manufacturing` requirements (HL1)
2. Totally `open-source` - well-documented source code, BoM, schematics, etc. as well as manufacturing, and assembly processes (HL1)
3. Easy/reliable to source parts (HL1)
4. Accessible, justifiable cost (HL1, HL2)
5. Ease of `troubleshooting`, straightforward solutions (HL1, HL3, HL9)
6. Standalone/`offline` operation (HL1, HL3, HL9)
7. Intuitive and low-complexity `assembly` (HL2)
8. Option for purchasing full `kits`, "minerals"-only kits (electronics AND not commonly manufacturable parts, i.e PCB) (HL2)
9. Option for singular part purchasing (i.e. bare PCB, singular sensor, etc.) (HL2, HL3)
10. `Sense` current temperature, humidity, air quality, and plant mass (HL4)
11. Timed `camera` capture and/or live feed (HL4)
12. Control internal temperature, humidity, lighting, and watering (HL5)
13. Networked and/or `cloud` operation (HL8)

# Metrics

> How do we measure the degree to which our objectives are being achieved? How do we compare candidate designs?

1. Likelihood that a makerspace, school, university, or research lab might have access to the tools necessary for `manufacturing` (LL1)
2. `Licensing` freedom (LL2)
3. Readability, scope, and detail of `documentation` (LL2, LL5)
4. Accessibility of documentation in terms of `hosting` (LL2, LL6)
5. Accessibility, shipping time/cost, and variety of parts `distributors` (LL3)
6. Reliability of part `availability` (LL3)
7. Total `cost` of materials (LL4)
8. Detail of device `debug` messaging (LL5)
9. Scope of troubleshooting, detail of solutions (LL5)
10. Likelihood that a household might have access to the tools necessary for `assembly` (LL7)
11. Untrained assembly `time` (LL7)

## Criteria and Constraints

> What are the **mandatory optima** (constraints) and **suggested criteria** for design?

1. ***User*** manufacturing `must` rely only on: 3D printers, laser cutters, CNC machines, soldering, handtools. ***Pre-manufactured*** components *may* rely on more complex means (i.e. PCB manufacturing) (M1)
2. `Must` use **MIT** license (M2)
3. Documentation `should` cover ***all*** framing, diverging, and converging reasoning (M3)
4. Documentation `must` provide reference to any source material (M3)
5. ***All*** code, 3D model files, electronic schematics, bill of materials (BoM) and any other manufacturing specs `must` be made readily available ***and*** updated regularly (M3, M4)
6. `Must` be hosted on **GitHub** (M4)
7. `Must` provide detailed parts BoM for **DigiKey**, one reliable USA distributor and one international/backup distributor (M5, M6)
8. Parts `must` be in active production (M6)
9. Total cost of materials `should` be lowered while not sacrificing quality, and `must` not exceed 500CAD (M7)
10. Debug messaging `should` be delivered in a variety of ways (i.e. onboard LEDs, onscreen log, email, push notification, etc.) in accordance with the abstraction layer that fails (M8)
11. Troubleshooting `should` cover all potential device issues, including those not indicated by debug channels outlined in C10 (M9)
12. Assembly `must` only rely on screwdrivers, allan keys, cutters, and pliers as tools (M10)
13. Assembly kits `should` provide necessary tools (i.e. those listed in C10) (M10)
14. Untrained assembly time `should` be minimized, and `must` not exceed 4 hours (M11)

<!-- TODO: more device operation-specific metrics? -->