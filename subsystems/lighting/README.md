# PeaPod Lighting Subsystem

# Assembly

## Materials

### Purchased

<!-- TODO: Cables?? -->

See `./hardware/lighting_bom_purchase.csv` for the purchase bill of materials. See `./hardware/gerber/` for PCB manufacturing files (NOTE: STENCIL NECESSARY). See `./hardware/lighting_bom_components.csv` for the electronic components bill of materials (DigiKey).

### Tools

Required:
- Soldering Iron, Solder, and Flux
- Wire Strippers (20-30 AWG)
- Reflow Oven (or hot air rework station) and solder paste
- Putty Scraper

Optional, but recommended:
- Helping Hands (for soldering)
- Multimeter (for testing connections)
- Oscilloscope (for testing signal integrity)
- Luminous Flux Meter (for measuring light output)

## Instructions

1. Using a putty scraper, apply solder paste to the PCB using the stencil.
2. Place the surface-mount components (signal connector, resistor, and LEDs) on the PCB according to the silkscreen.
3. Reflow the PCB in a reflow oven (or use a hot air rework station) to solder the surface-mount components.
4. Solder the through-hole components (power connector and LED driver) to the PCB according to the silkscreen.

## Testing