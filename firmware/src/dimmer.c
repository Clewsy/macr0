#include "dimmer.h"

void dimmer_init(void)
{
	DIMMER_DDR &= ~(1 << DIMMER_BUTTON);	// Set button pin as input.
//	DIMMER_PORT |= (1 << DIMMER_BUTTON);	// Enable internal pull-ups.
	DIMMER_PCMSK |= (1 << DIMMER_PCINT);	// Enable pin-change interrupt for selected pin.
}

void dimmer_enable(void)
{
	DIMMER_PCICR |= (1 << DIMMER_PCIE);	// PCICR: Pin Change Interrupt Control Register.
}

void dimmer_disable(void)
{
	DIMMER_PCICR &= ~(1 << DIMMER_PCIE);	// PCICR: Pin Change Interrupt Control Register.
}

bool dimmer_check(void)
{
	if(~DIMMER_PINS & (1 << DIMMER_BUTTON))	return(true);
	else					return(false);
}
