# PeaPod Subsystems

All subsystems will follow the same folder structure:

- `assets/` - various graphical/etc. assets (diagrams, etc. excluding pictures of schematics/PCB layouts)
- `hardware/`
- `software/`

In addition, each subsystem should have its own `README.MD` with the following info:

- Purchasing
- Assembly w/ photos
- Testing (process and results)

## Hardware

Includes all bills of materials (BOM), 3D printing files, PCB files, etc.

Subdirectories:

- `boms/` - Bills of Materials (CSV files)
- `fabrication/` - Fabricated part files (i.e. 3D printing files)
- `schematics/` - PCB schematics (.SCH and .BRD files and photos of each)
- `gerber/` - Gerber files (PCB manufacturing)

## Software

Includes all C/C++ libraries and Arduino testing scripts.

Subdirectories:

- `src/` - Libraries
- `tests/` - Test scripts

# Development

Component BOMs downloaded from [DigiKey](https://www.digikey.ca) (can be directly uploaded for ordering). All other BoM types (fabrication, purchase, etc.) copied from the [BoM Templates](/.github/BOM_TEMPLATES/). 3D models, PCB and schematic docs are made using [AutoDesk Fusion 360](https://www.autodesk.ca/en/products/fusion-360/overview).

## Requirements

- Fusion 360 or your 3D editor of choice
- EasyEDA or your PCB/schematic editor(s) of choice
- Excel/Google Sheets or your CSV reader/spreadsheet software of choice
