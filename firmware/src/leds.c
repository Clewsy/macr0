#include "leds.h"

uint8_t led_mode = 1;		// Default led_mode - set at init.
int8_t led_pulse_direction = 1;	// Initiaisel pulse direction.

// Each "mode" is defined by a uint8_t for brightness and a uint16_t for pulse speed.
// initial_brightness:	The PWM value to send to the LEDs before setting the pulse speed, 0 to 255.
//			Note, the LEDs are active low, so full brightness requires a value of 0xFF.
// pulse_speed:		When the pulse timer counter reaches this value, an interrupt is triggered
//			that changes the PWM value.  0 to 65535.  Set to zero for no pulse effect.
const struct mode modes[] = 
{
	{ .initial_brightness = 255, 	.pulse_speed = 0},		// Mode 00
	{ .initial_brightness = 254, 	.pulse_speed = 0},		// Mode 01
	{ .initial_brightness = 253, 	.pulse_speed = 0},		// Mode 02
	{ .initial_brightness = 251, 	.pulse_speed = 0},		// Mode 03
	{ .initial_brightness = 247, 	.pulse_speed = 0},		// Mode 04
	{ .initial_brightness = 239, 	.pulse_speed = 0},		// Mode 05
	{ .initial_brightness = 223, 	.pulse_speed = 0},		// Mode 06
	{ .initial_brightness = 191, 	.pulse_speed = 0},		// Mode 07
	{ .initial_brightness = 127, 	.pulse_speed = 0},		// Mode 08
	{ .initial_brightness = 0, 	.pulse_speed = 0},		// Mode 09
	{ .initial_brightness = 0, 	.pulse_speed = PULSE_FAST},	// Mode 10
	{ .initial_brightness = 0, 	.pulse_speed = PULSE_SLOW},	// Mode 11
};

// Initialise the AVR registers for controlling the LEDs.
// The hardware configuration has the pwm pin connected to a PNP transistor that controls all LEDs on the anode side.
void leds_init(void)
{
	////////Initialise the pwm timer.
	PWM_DDR |= (1 << PWM_PIN);	// Set the PWM pin as an output.
	// WGM[1:0] set to 00 : Fast PWM mode.
	// COM[1:0] set to 10 : Clear on compare match, set at bottom (TCNT4 = 0x00), OC4A pin connected.
	// PWM_AE set to enable PWM on comparator A.
	// CS[3:0] set to 0111 : clk = CK/64 = 250,000Hz.  PWM_freq = clk/256 = 964.5625Hz
	PWM_TCCRA |= ((1 << PWM_COM1) | (1 << PWM_AE));
	PWM_TCCRB |= ((0 << PWM_CS3) | (1 << PWM_CS2) | (1 << PWM_CS1) | (1 << PWM_CS0));
	PWM_DDR |= (1 << PWM_PIN);	// Set as an output the pin to which the LEDs transistor is connected.
	PWM_SET = PWM_INIT_VAL;		// When initialised, duty cycle will be at a known value.

	////////Initialise the pulse timer.
	// WGM[2:0] set to 0100 : CTC mode, counts from 0 to value of output compare register.
	// COM[1:0] set to 00 : Normal pin modes - not connected to timer.
	// CS[2:0] set to 010 : clk/8 (from prescaler).
	PULSER_TCCRB |= ((1 << PULSER_WGM2) | (1 << PULSER_CS1));

	// Set the default led mode.
	leds_set_mode();
}

// Set the current duty cycle (0 to 255).
void leds_pwm_set(uint8_t duty)
{
	PWM_SET = duty;
}

// Get the current duty cycle (0 to 255).
uint8_t leds_pwm_get(void)
{
	return(PWM_SET);
}

// Enable the pulse effect by enabling the output compare interrupt.
void leds_pulse_enable(uint8_t enable)
{
	if(enable)	PULSER_TIMSK |= (1 << PULSER_IE);	// Enable the output compare interrupt.
	else		PULSER_TIMSK &= ~(1 << PULSER_IE);	// Disable the output compare interrupt.
}

// Set the counter value that will trigger the interrupt.  Also disable the interrupt if the selected speed is zero.
void leds_pulse_speed_set(uint16_t speed)
{
	PULSER_SET_REG = speed;

	if(speed)	leds_pulse_enable(true);
	else		leds_pulse_enable(false);	// Enable pulse only if speed is not zero.
}

// Using the look-up table ("modes"), set the desired brightness (pwm value) and pulse speed.
void leds_set_mode(void)
{
	leds_pwm_set(modes[led_mode].initial_brightness);
	leds_pulse_speed_set(modes[led_mode].pulse_speed);
}

// Update the pwm value when the pulse effect interrupt is triggered.
void leds_handle_pulser_interrupt(void)
{
	uint8_t current_pwm = leds_pwm_get();				// Check the current pwm value.

	if	(current_pwm == 255)	led_pulse_direction = -1;	// Reverse the pulse direction at either end of the count.
	else if	(current_pwm == 0)	led_pulse_direction = 1;

	leds_pwm_set(current_pwm + led_pulse_direction);		// Update the led brightness.

}

// Cycle through the various led modes.
void leds_change_mode(void)
{
	if(led_mode == (NUM_MODES - 1))	led_mode = 0;	// Cycle back to the first mode if last mode reached.
	else				led_mode++;	// Else just increment to the next mode.

	leds_set_mode();
}
