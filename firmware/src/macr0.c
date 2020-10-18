#include "macr0.h"

// This interrupt sub-routine is triggered by a counter configured to vary the PWM signal used to control the brightness
// of the LEDs.  When enabled this creates a pulsing effect with the LEDs.
ISR(PULSER_INT_VECTOR)
{
	leds_handle_pulser_interrupt();
}

// This interrupt sub-routine is trigerred when the dimmer/brightness button is pressed.  Pressing the button cycles the
// pwm duty cycle through various values, effectively stepping through various brigntness values of the LEDs.  Note the
// PWM signal controls a PNP transistor on the anode side of the LEDs, therefore duty cycle 255 = off.
ISR(BUTTON_PCI_VECTOR)
{
	leds_handle_button_interrupt();
}

// Initialise the hardware peripherals.
void hardware_init(void)
{
	leds_init();		// Defined in leds.c
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
