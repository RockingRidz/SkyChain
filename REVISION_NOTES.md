# Revision and validation notes

## PCB and firmware corrections

- USB-C J1 now faces outward; its mating edge is flush with the PCB edge at Y = 119.50 mm. Footprint origin: X = 169.18 mm, Y = 115.85 mm, rotation = 0 degrees.
- SW2 uses GPIO10 and SW3 uses GPIO3. SW1 remains reset. Firmware matches these assignments and no longer uses GPIO12/13 for buttons.
- Removed unintended connections from USB VBUS to IO2 and CC2 to IO5.
- Regenerated copper zones, Gerbers and both plated/non-plated drill files from the revised board.

## Checks and limitations

KiCad DRC reports zero unconnected pads and zero footprint parity errors. Its only reported violation is the J1 library-footprint mismatch warning caused by trimming silkscreen at the board edge. See pcb/DRC.rpt, including its list of ignored checks.

ERC still reports a pre-existing Q1 emitter/power-flag conflict and an excluded regulator/3V3 output warning; see pcb/ERC.rpt. These changes do not constitute electrical or physical prototype validation. Firmware has not been compiled or tested on hardware.

## Enclosure: fit not approved

The CAD sources and case/lid meshes in cad/ are preserved unchanged. Do not treat the existing STL as verified for the revised PCB.

The measured front USB opening is 10 mm wide by 4 mm high. With the PCB centered in the cavity, the connector center is approximately 0.93 mm off the opening center, and the shell extends approximately 0.40 mm beyond one side of the opening. The mouth is flush with the PCB, but remains approximately 2.25 mm behind the outer case wall under this positioning assumption. Cable housing clearance therefore needs an enlarged/aligned opening and a physical or assembled-CAD fit check. Board seating and mounting height have not been confirmed.

Old PCB STEP/STL exports were removed because they no longer represented the revised board. Source enclosure CAD was retained so the opening can be revised without losing design history.
