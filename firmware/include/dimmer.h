#include <avr/io.h>
#include <stdbool.h>

// Defines.
#define DIMMER_PORT		PORTB		// Port register containing dimmer button pin (for setting DDR).
#define DIMMER_DDR		DDRB		// Data direction register for relevant port.
#define DIMMER_PINS		PINB		// Pins register ccontaining dimmer button pin (for readin pin state).
#define DIMMER_PCICR		PCICR		// Pin-change interrupt control register.
#define DIMMER_PCMSK		PCMSK0		// Pin-change mask.
#define DIMMER_PCIE		PCIE0		// Pin-change interrupt enable bit.
#define DIMMER_PCINT		PCINT0		// Pin-change interrupt bit.
#define DIMMER_PCI_VECTOR	PCINT0_vect	// Pin-change Interrupt sub-routine vector.
#define DIMMER_BUTTON		PB0		// PB0 is also PCINT0 - Pin Change Interrupt Pin 0

// Function prototypes.
void dimmer_init(void);
void dimmer_enable(void);
void dimmer_disable(void);
bool dimmer_check(void);
