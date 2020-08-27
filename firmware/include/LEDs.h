//The following definitions and functions are used to implement the two LEDs on the Pololu A* micro board.
//One LED is green and is active low on port D, pin 5.  The other is orange and active high on port C, pin 7.

#define LED_GREEN		(1 << PD5)	//Green LED
#define LED_ORANGE		(1 << PC7)	//Orange LED

#define LED_GREEN_DDR		DDRD
#define LED_ORANGE_DDR		DDRC

#define LED_GREEN_PORT		PORTD
#define LED_ORANGE_PORT		PORTC

#define LED_GREEN_OFF		LED_GREEN_PORT |= LED_GREEN
#define LED_ORANGE_OFF		LED_ORANGE_PORT &= ~LED_ORANGE

#define LED_GREEN_ON		LED_GREEN_PORT &= ~LED_GREEN
#define LED_ORANGE_ON		LED_ORANGE_PORT |= LED_ORANGE

//Initialise the LEDs by setting data direction registers as outputs and turning both LEDs off.
static inline void LEDS_Init(void)
{
	LED_GREEN_DDR |= LED_GREEN;
	LED_ORANGE_DDR |= LED_ORANGE;

	LED_GREEN_OFF;
	LED_ORANGE_OFF;
}

//Disable the LEDs by resetting the data direction registers so that the relevant pins become inputs.
static inline void LEDS_Disable(void)
{
	LED_GREEN_DDR &= ~LED_GREEN;
	LED_ORANGE_DDR &= ~LED_ORANGE;

	LED_GREEN_PORT &= ~LED_GREEN;
	LED_ORANGE_PORT &= ~LED_ORANGE;
}

//Turn on specified LEDs regardless of their current state or the state of any others.
static inline void LEDs_TurnOnLEDS(const uint8_t LEDMask)
{
	LED_GREEN_PORT &= ~(LEDMask & LED_GREEN);
	LED_ORANGE_PORT |= (LEDMask & LED_ORANGE);
}

//Turn off specified LEDs regardless of their current state or the state of any others.
static inline void LEDs_TurnOffLEDS(const uint8_t LEDMask)
{
	LED_GREEN_PORT |= (LEDMask & LED_GREEN);
	LED_ORANGE_PORT &= ~(LEDMask & LED_ORANGE);
}

//Turn on specified LEDs and turn off non-specified LEDs.
static inline void LEDS_SetAllLEDS(const uint8_t LEDMask)
{
	LED_GREEN_PORT = ((LED_GREEN_PORT | LED_GREEN) & ~LEDMask);
	LED_ORANGE_PORT = ((LED_ORANGE_PORT & ~LED_ORANGE) | (LEDMask & LED_ORANGE));
}
