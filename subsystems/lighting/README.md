# PeaPod Lighting Subsystem

See `./REQUIREMENTS.md` for the subsystem requirements.

Provides controlled light to the plant canopy and supports programmable photoperiods (on/off), light intensity (dimming), and spectral channel control via Adafruit PCA9685 PWM dimming of MEAN WELL LDD-L constant-current LED drivers powering CREE XLAMP LEDs with a variety of spectra, including PAR and white for imaging. Power regulator, I2C cables, and PCBs are documented swappable components.

# Production

## Assembly

### Bills of Materials

<!-- TODO: Cables?? -->

See `./hardware/lighting_bom_purchase.csv` for the purchase bill of materials. See `./hardware/gerber/` for PCB manufacturing files (NOTE: STENCIL NECESSARY). See `./hardware/lighting_bom_components.csv` for the electronic components bill of materials (DigiKey).

1. 1 x Lighting BOM Components per led board
2. 1 x Lighting BOM Purchase per led board

### Tools and Equipment

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

### Power Test

**Protocol**: Connect the 24V power supply (+24VDC and GND) to the power connector on the PCB.

**Validation**: Nothing happens. Test points TP3 and TP4 read +24V and 0V, respectively.

**Failure Protocol**: 

1. *Smoke, Sparks or Unusual Heat/Odors*: Immediately disconnect the power supply. Check the PCB for solder bridges or shorts. If issue persists, check the power supply and ensure it is functioning correctly.

2. *Incorrect Voltage Readings*: Check the power supply connections and ensure the PCB is properly soldered.

### LED Light Test

**Protocol**: Connect the 24V power supply (+24VDC and GND) to the power connector on the PCB. Flash an Arduino-compatible microcontroller with the `./software/tests/Test_SingleWave/Test_SingleWave.ino` sketch. Connect the microcontroller to the signal connector on the PCB (Digital Pin 3 and GND). Observe the behavior of the LEDs.

**Validation**: The LEDs should light up in a sinusoidal wave pattern, with the brightness varying smoothly over time.

**Failure Protocol**: Check the power supply connections and ensure the PCB is properly soldered. Check test points TP3 and TP4 for correct voltage levels. Check the microcontroller connections to the signal connector and ensure the correct pin is used. Check test point TP5 for PWM signal using an oscilloscope. If issue persists, check the microcontroller code and ensure it is functioning correctly.