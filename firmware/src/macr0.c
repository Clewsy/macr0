#include "macr0.h"

ISR(DIMMER_PCI_VECTOR)
{
	_delay_ms(20);	// Button de-bounce.

	if(dimmer_check())	// If the button is still pressed (i.e. not a bounce).
	{
		if(cat4104_get() == 250)	cat4104_set(0);
		else				cat4104_set(cat4104_get() + 50);

		while(dimmer_check()) {}	// Wait until the button is released.
	}
}


void hardware_init(void)
{
	cat4104_init();
	dimmer_init();
	dimmer_enable();
	keyscan_init();

	SetupHardware();	// Specifically, keyboard HID USB hardware.  Function defined in Keyboard.c
}


/** Main program entry point. This routine configures the hardware required by the application, then
 *  enters a loop to run the application tasks in sequence.
 */
int main(void)
{

	hardware_init();
//	LEDs_SetAllLEDs(LEDMASK_USB_NOTREADY);
	GlobalInterruptEnable();

	for (;;)
	{
		HID_Task();
		USB_USBTask();
	}
}
