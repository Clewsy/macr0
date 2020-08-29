#include <avr/io.h>

#define PWM_PORT	PORTC
#define PWM_DDR		DDRC
#define PWM_PIN		PC7

#define PWM_SET		OCR4A		// Output compare register (writing 8-bits to here sets the pulse-width).
#define PWM_TCCRA	TCCR4A		// Timer/Counter Control Register
#define PWM_TCCRB	TCCR4B		// Timer/Counter Control Register
#define PWM_TCCRC	TCCR4C		// Timer/Counter Control Register
#define PWM_TCCRD	TCCR4D		// Timer/Counter Control Register
#define PWM_TCCRE	TCCR4E		// Timer/Counter Control Register
#define PWM_COM0	COM4A0		// Compare Output Mode Bit 0
#define PWM_COM1	COM4A1		// Compare Output Mode Bit 1
#define PWM_AE		PWM4A		// PWM on comparator A Enable
#define PWM_WGM0	WGM40		// Waveform Generation Mode Bit 0
#define PWM_WGM1	WGM41		// Waveform Generation Mode Bit 1
#define PWM_CS0		CS40		// Clock Select Bit 0
#define PWM_CS1		CS41		// Clock Select Bit 1
#define PWM_CS2		CS42		// Clock Select Bit 2
#define PWM_CS3		CS43		// Clock Select Bit 3

#define PWM_INIT_VAL	255		// Initial duty cycle value.

// Function prototypes.
void cat4104_init(void);
void cat4104_set(uint8_t duty);
