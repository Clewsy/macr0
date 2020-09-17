#include "macr0.h"

// This interrupt sub-routine is trigerred when the dimmer/brightness button is pressed.
// Pressing the button cycles the pwm duty cycle through various values, effectively stepping through various brigntness values of the LEDs.
// Note the PWM signal controls a PNP transistor on the anode side of the LEDs, therefore duty cycle 255 = off and duty cycle 0 = full brightness.
ISR(DIMMER_PCI_VECTOR)
{
	_delay_ms(DEBOUNCE_MS);	// Button de-bounce.

	if(dimmer_check())	// If the button is still pressed (i.e. not a bounce or a release).
	{
		// PWM value ranges from 5 to 255 in multiples of 25.
		if(leds_get() == 255)	leds_set(5);
		else			leds_set(leds_get() + 25);
	}
}

// Initialise the hardware peripherals.
void hardware_init(void)
{
	leds_init();
	dimmer_init();
	keyscan_init();

	SetupHIDHardware();	// Keyboard HID USB hardware.  Function defined in Keyboard.c
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
