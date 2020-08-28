#include <avr/io.h>
#include <stdbool.h>

#define DIMMER_PORT		PORTB
#define DIMMER_DDR		DDRB
#define DIMMER_PINS		PINB
#define DIMMER_PCICR		PCICR
#define DIMMER_PCIE		PCIE0
#define DIMMER_PCMSK		PCMSK0
#define DIMMER_PCINT		PCINT0
#define DIMMER_PCI_VECTOR	PCINT0_vect
#define DIMMER_BUTTON		PB0	//PCINT0 - Pin Change Interrupt Pin 0

void dimmer_init(void);
void dimmer_enable(void);
void dimmer_disable(void);
bool dimmer_check(void);
//ISR(DIMMER_PCI_VECTOR);
