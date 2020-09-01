#include "macr0.h"

// This interrupt sub-routine is trigerred when the dimmer/brightness button is pressed.
// Pressing the button cycles the pwm duty cycle through various values, effectively stepping through various brigntness values of the LEDs.
ISR(DIMMER_PCI_VECTOR)
{
	_delay_ms(DEBOUNCE_MS);	// Button de-bounce.

	if(dimmer_check())	// If the button is still pressed (i.e. not a bounce or a release).
	{
		if(cat4104_get() == 250)	cat4104_set(0);
		else				cat4104_set(cat4104_get() + 50);
	}
}

// Initialise the hardware peripherals.
void hardware_init(void)
{
	cat4104_init();
	dimmer_init();
	keyscan_init();

	SetupHIDHardware();	// Keyboard HID USB hardware.  Function defined in Keyboard.c
}


// Main program entry point. This routine configures the hardware required by the application, then enters a loop to run the application tasks in sequence.
 int main(void)
{
	hardware_init();
//	LEDs_SetAllLEDs(LEDMASK_USB_NOTREADY);
	GlobalInterruptEnable();

	for (;;)
	{
		HID_Task();	// In Keyboard.c
		USB_USBTask();	// In the lufa library.
	}
}
