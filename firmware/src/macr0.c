#include "macr0.h"

// This interrupt sub-routine is triggered by a counter configured to vary the PWM signal used to control the brightness of the LEDs.
// It creates a pulsing effect when enabled.
ISR(PULSER_INT_VECTOR)
{
	leds_handle_pulser_interrupt();
}


// This interrupt sub-routine is trigerred when the dimmer/brightness button is pressed.
// Pressing the button cycles the pwm duty cycle through various values, effectively stepping through various brigntness values of the LEDs.
// Note the PWM signal controls a PNP transistor on the anode side of the LEDs, therefore duty cycle 255 = off and duty cycle 0 = full brightness.
ISR(DIMMER_PCI_VECTOR)
{
	// Button de-bounce.
	_delay_ms(DEBOUNCE_MS);

	// If the button is still pressed (i.e. not a bounce or a release), change the mode.
	if(dimmer_check()) leds_change_mode();
}

// Initialise the hardware peripherals.
void hardware_init(void)
{
	leds_init();		// Defined in leds.c
	dimmer_init();		// Defined in dimmer.c
	keyscan_init();		// Defined in keyscan.c
	SetupHIDHardware();	// Defined in Keyboard.c
}


// Main program entry point.
 int main(void)
{
	hardware_init();
	GlobalInterruptEnable();

	while(true)
	{
		HID_Task();	// In Keyboard.c
		USB_USBTask();	// In the lufa library.
	}
}
