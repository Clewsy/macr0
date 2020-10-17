#include <avr/io.h>
#include <stdbool.h>

#define PWM_PORT	PORTC	// The port register that includes the pwm pin.
#define PWM_DDR		DDRC	// Data direction register that corresponds to the relevant port.
#define PWM_PIN		PC7	// The pin to which the PWM signal will be connected.

#define PWM_SET		OCR4A	// Output compare register (writing 8-bits to here sets the pulse-width).
#define PWM_TCCRA	TCCR4A	// Timer/Counter Control Register
#define PWM_TCCRB	TCCR4B	// Timer/Counter Control Register
#define PWM_TCCRC	TCCR4C	// Timer/Counter Control Register
#define PWM_TCCRD	TCCR4D	// Timer/Counter Control Register
#define PWM_TCCRE	TCCR4E	// Timer/Counter Control Register
#define PWM_COM0	COM4A0	// Compare Output Mode Bit 0
#define PWM_COM1	COM4A1	// Compare Output Mode Bit 1
#define PWM_AE		PWM4A	// PWM on comparator A Enable
#define PWM_WGM0	WGM40	// Waveform Generation Mode Bit 0
#define PWM_WGM1	WGM41	// Waveform Generation Mode Bit 1
#define PWM_CS0		CS40	// Clock Select Bit 0
#define PWM_CS1		CS41	// Clock Select Bit 1
#define PWM_CS2		CS42	// Clock Select Bit 2
#define PWM_CS3		CS43	// Clock Select Bit 3

#define PWM_INIT_VAL	230	// Initial duty cycle value (0 to 255).





// Definitions used for initiatilising and controling the pulse timer. 
#define PULSER_TCCRA		TCCR1A			// Timer/Counter Control Register A
#define PULSER_TCCRB		TCCR1B			// Timer/Counter Control Register B
#define PULSER_WGM3		WGM13			// Timer/Counter Waveform Generation Mode Bit 3
#define PULSER_WGM2		WGM12			// Timer/Counter Waveform Generation Mode Bit 2
#define PULSER_WGM1		WGM11			// Timer/Counter Waveform Generation Mode Bit 1
#define PULSER_WGM0		WGM10			// Timer/Counter Waveform Generation Mode Bit 0
#define PULSER_CS2		CS12			// Timer/Counter Clock Select Bit 2
#define PULSER_CS1		CS11			// Timer/Counter Clock Select Bit 1
#define PULSER_CS0		CS10			// Timer/Counter Clock Select Bit 0
#define PULSER_SET_REG		OCR1A			// Timer/Counter Output Compare Register
#define PULSER_TIMSK		TIMSK1			// Timer/Counter Timer Interrupt Mask Register
#define PULSER_IE		OCIE1A			// Timer Output Compare Interrupt Enable Bit.
#define PULSER_INT_VECTOR	TIMER1_COMPA_vect	// Interrupt subroutine name.
//#define PULSER_SPEED		8000			// Write to PULSER_SET_REG to set the pulse speed.


// Various led modes.
#define PULSE_FAST	8000
#define PULSE_SLOW	32000
#define NUM_MODES	(sizeof(modes) / sizeof modes[0])	// The total numbber of modes per the modes[] array.

// Declarations:
uint8_t led_mode;
int8_t led_pulse_direction;
void leds_init(void);
void leds_pwm_set(uint8_t duty);
uint8_t leds_pwm_get(void);
void leds_pulse_enable(uint8_t enable);
void leds_set_mode(void);
void leds_handle_pulser_interrupt(void);
void leds_change_mode(void);

struct mode
{
	uint8_t initial_brightness;
	uint16_t pulse_speed;
};

