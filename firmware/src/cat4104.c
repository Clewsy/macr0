#include "cat4104.h"

void cat4104_init(void)
{
	PWM_DDR |= (1 << PWM_PIN);

	////////Initialise the pwm timer.
	// WGM[1:0] set to 00 : Fast PWM mode.
	// COM[1:0] set to 10 : Clear on compare match, set at bottom (TCNT4 = 0x00), OC4A pin connected.
	// CS[3:0] set to 0001 : clk = CK (no prescaler).
	// PWM_AE set to enable PWM on comparator A.
	PWM_TCCRA |= ((1 << PWM_COM1) | (1 << PWM_AE));
	PWM_TCCRB |= (1 << PWM_CS0);

	PWM_DDR |= (1 << PWM_PIN);	// Set as an output the pin to which the LED is connected.
	PWM_SET = 0x01;			// When initialised, duty cycle will be at a known value.

}

void cat4104_set(uint8_t duty)
{
	PWM_SET = duty;
}