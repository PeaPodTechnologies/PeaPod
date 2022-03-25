# PeaPod Lighting Subsystem

## Purchasing

<!-- TODO: Notes on purchasing in main README (i.e. suppliers, process) -->

- PCBs - `./hardware/boms/lighting_purchase_bom.csv`
- Lighting Cables - `./hardware/boms/lighting_component_bom.csv`
- 5x Lighting LED Board BoM - `./hardware/led/boms/lighting_led_component_bom.csv`
- 1x Lighting Driver Board BoM - `./hardware/driver/boms/lighting_power_component_bom.csv`

## Assembly

<!-- TODO: Photos for board mount positioning, PCB orientation -->
<!-- TODO: Tools and materials needed? -->

1. Assemble all PCBs (see BoMs for reference designators)
2. Fasten LED and driver board mounts to lighting tray frame
3. Fasten LED and driver boards to appropriate board mounts, ensuring proper LED board orientation for daisy-chaining
4. Connect all cables:
   1. Daisy-chained LED board power cables (driver `+` -> LED board 1 `+`, LED board 1 `-` -> LED board 2 `+`, ..., LED board 5 `-` -> driver `-`)
   2. Power board DC jack (+48VDC)
   3. Signal cable (driver `SIGNAL` -> motherboard `LIGHTING`)

## Testing

<!-- TODO: Single-board and channel-control testing, integration with PIO unit tests -->
<!-- TODO: Troubleshooting? -->

### Driver Board Power

1. Disconnect cables from driver board except DC jack
2. Power on the power supply (+48VDC)
3. Test the voltage difference between the VCC and GND test points using a voltmeter