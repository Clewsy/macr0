#include <avr/io.h>
//#include <LUFA/Drivers/USB/USB.h>	// Included for the scancode definitions.
#include "keymap.h"

#define KEYS_PORT	PORTD
#define KEYS_PINS	PIND
#define KEYS_DDR	DDRD
#define ROW_1		PD0
#define ROW_2		PD1
#define COL_1		PD2
#define COL_2		PD3
#define NO_KEY		0x00	//Used to indicate a no-key keystroke event.
#define NO_MODIFIER	0x00	//Used to indicate a no-modifier keystroke event.


void keyscan_init(void);
char keyscan_get_key(void);
uint8_t keyscan_char_to_key(char c);

