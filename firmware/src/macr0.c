#include "macr0.h"



uint8_t dim_value = 250;
ISR(DIMMER_PCI_VECTOR)
{
	_delay_ms(20);	// Button de-bounce.

	if(dimmer_check())
	{
		dim_value += 50;
		if(dim_value == (uint8_t)(250+50)) dim_value = 0;

		cat4104_set(dim_value);

		while(dimmer_check()) {}	// Wait until the button is released.
	}
}


/** Main program entry point. This routine configures the hardware required by the application, then
 *  enters a loop to run the application tasks in sequence.
 */
int main(void)
{

cat4104_init();
dimmer_init();
dimmer_enable();


	SetupHardware();

//	LEDs_SetAllLEDs(LEDMASK_USB_NOTREADY);
	GlobalInterruptEnable();

	for (;;)
	{
		HID_Task();
		USB_USBTask();
	}
}
