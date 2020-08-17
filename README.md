# macr0

4-button HID compliant USB macro keypad.

This is a test project to try out a few things before attempting a larger-scale project.  Some proofs of concepts - things I haven't done before.

Goals for this project:
- [ ] HID compliant USB peripheral using an ATmega32U4 microcontroller.
- [ ] Keys will be matrixed (even though the mcu has plenty of spare IO.)
- [ ] Keys will be mechanical Gateron switches - these are pin-compatible Cherry MX clones.
- [ ] Keys will all be LED back-lit using a constant-current LED driver (even though they could be easily driven directly by the mcu).
- [ ] Back-light illumination will be adjustable.
- [ ] Connectivity (and power) will be via a USC-C connector, configured for USB 2.0.
