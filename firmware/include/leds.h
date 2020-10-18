#include <avr/io.h>
#include <stdbool.h>	// Needed for using true/false booleans.
#include <util/delay.h>	// Needed for using _delay_ms() function.

// Definitions used for initiatilising and controling the led control button. 
#define BUTTON_PIN		PB0		// PB0 is also PCINT0 - Pin Change Interrupt Pin 0
#define BUTTON_PORT		PORTB		// Port register containing dimmer button pin (for setting DDR).
#define BUTTON_DDR		DDRB		// Data direction register for relevant port.
#define BUTTON_PINS		PINB		// Pins register ccontaining dimmer button pin (for readin pin state).
#define BUTTON_PCICR		PCICR		// Pin-change interrupt control register.
#define BUTTON_PCMSK		PCMSK0		// Pin-change mask.
#define BUTTON_PCIE		PCIE0		// Pin-change interrupt enable bit.
#define BUTTON_PCINT		PCINT0		// Pin-change interrupt bit.
#define BUTTON_PCI_VECTOR	PCINT0_vect	// Pin-change Interrupt sub-routine vector.
#define BUTTON_DEBOUNCE_MS	40		// Button de-bounce duration in milliseconds.


// Definitions used for initiatilising and controling the pwm output pin. 
#define PWM_PIN		PC7	// The pin to which the PWM signal will be connected.
#define PWM_PORT	PORTC	// The port register that includes the pwm pin.
#define PWM_DDR		DDRC	// Data direction register that corresponds to the relevant port.
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

// Mode definitions.
#define NUM_MODES	(sizeof(modes) / sizeof modes[0])	// Macro for returning the total number of modes.
#define START_MODE	10					// The led mode at start-up.

// Declarations:
uint8_t led_mode;
int8_t led_pulse_direction;
void leds_init(void);
void leds_pwm_set(uint8_t duty);
uint8_t leds_pwm_get(void);
void leds_pulse_enable(bool enable);
void leds_set_mode(uint8_t mode);
void leds_handle_pulser_interrupt(void);
void leds_change_mode(void);
void leds_button_enable(bool enable);
bool leds_button_state(void);
void leds_handle_button_interrupt(void);

struct mode
{
	uint8_t initial_brightness;
	uint16_t pulse_speed;
};
