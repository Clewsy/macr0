#include "cat4104.h"

// Initialise the AVR registers for controlling the CAT4104.
void cat4104_init(void)
{
	PWM_DDR |= (1 << PWM_PIN);	// Set the PWM pin as an output.

	////////Initialise the pwm timer.
	// WGM[1:0] set to 00 : Fast PWM mode.
	// COM[1:0] set to 10 : Clear on compare match, set at bottom (TCNT4 = 0x00), OC4A pin connected.
	// PWM_AE set to enable PWM on comparator A.
	// CS[3:0] set to 1111 : clk = CK/16384 = 250000Hz.  PWM_freq = clk/256 = 964.5625Hz (CAT4104 rated for PWM control frequency range 100Hz to 5000Hz).
	PWM_TCCRA |= ((1 << PWM_COM1) | (1 << PWM_AE));
	PWM_TCCRB |= ((0 << PWM_CS3) | (1 << PWM_CS2) | (1 << PWM_CS1) | (1 << PWM_CS0));

	PWM_DDR |= (1 << PWM_PIN);	// Set as an output the pin to which the LED is connected.
	PWM_SET = PWM_INIT_VAL;		// When initialised, duty cycle will be at a known value.
}

// Set the current duty cycle (0 to 255).
void cat4104_set(uint8_t duty)
{
	PWM_SET = duty;
}

// Get the current duty cycle (0 to 255).
uint8_t cat4104_get(void)
{
	return(PWM_SET);
}