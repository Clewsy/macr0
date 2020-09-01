#include <avr/io.h>
//#include <avr/interrupt.h>

#include "Keyboard.h"
//#include "keyscan.h"	// Included in Keyboard.h
//#include "keymap.h"// Included in keyscan.h
#include "dimmer.h"
#include "cat4104.h"

#define DEBOUNCE_MS 20	// Button de-bounce duration in milliseconds.