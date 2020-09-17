#include <avr/io.h>

#include "Keyboard.h"	// Pulls in all the lufs library defines.
//#include "keyscan.h"	// Included in Keyboard.h
//#include "keymap.h"	// Included in keyscan.h
#include "dimmer.h"	// Configure and enable an interrupt for a push button that will set the cat4104 PWM duty cycle.
#include "leds.h"	// Configure and set a pwm signal for controlling LED brightness via a cat4104 led driver.

#define DEBOUNCE_MS 20	// Button de-bounce duration in milliseconds (for dimmer push-button).