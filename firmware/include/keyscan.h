#include <avr/io.h>
//#include <LUFA/Drivers/USB/USB.h>	// Included for the scancode definitions.
#include <string.h>			// Included for memset function.
#include "keymap.h"

#define KEYS_PORT	PORTD	// Hardware Port definition for controlling scan rows.
#define KEYS_PINS	PIND	// Hardware Pins definition for reading column rows.
#define KEYS_DDR	DDRD	// Hardware data direction register for setting row pins as outputs and column pins as inputs.
#define ROW_1		PD0	// Pin connected to the first row.
#define ROW_2		PD1	// Pin connected to the second row.
#define COL_1		PD2	// Pin connected to the first column.
#define COL_2		PD3	// Pin connected to the second column.

#define MAX_KEYS	4	// Maximum number of simultaneous key-presses that can be detected (excluding media keys and modifier keys).

// Bit-shift definitions for the uint16_t media_keys integer:
#define MK_PLAY		 0
#define MK_PAUSE	 1
#define MK_FF		 2
#define MK_RW		 3
#define MK_NEXT		 4
#define MK_PREVIOUS	 5
#define MK_STOP		 6
#define MK_TOGGLE	 7
#define MK_MUTE		 8
#define MK_VOL_UP	 9
#define MK_VOL_DOWN	10

// See comments bellow for a breakdown of this struct.
typedef struct
{
	uint16_t media_keys;
	uint8_t modifier;
	uint8_t keys[MAX_KEYS];
} keyscan_report_t;

// Function declarations.
void keyscan_init(void);
void handle_key(char key, keyscan_report_t *keyscan_report);
void create_keyscan_report(keyscan_report_t *keyscan_report);


/* Breakdown of a keyscan_report:

modifier: 8 bits each represent the state of a modifier:
lsb	bit0	Left Control
	bit1	Left Shift
	bit2	Left Alt
	bit3	Left Gui
	bit4	Right Control
	bit5	Right Shift
	bit6	Right Alt
msb	bit7	Right Gui

keys[6]: each key is represented by a byte 0x00 to 0x65.
A maximum of six simultaneous key presses will be registered.

media_keys: 11 bits each represent the state of a media key:
lsb	bit00	Play 
	bit01	Pause 
	bit02	Fast Forward 
	bit03	Rewind 
	bit04	Next Track 
	bit05	Previous Track 
	bit06	Stop 
	bit07	Play/Pause (toggle) 
	bit08	Mute 
	bit09	Volume Up 
	bit10	Volume Down 
	bit11	n/a (ignored)
	bit12	n/a (ignored)
	bit13	n/a (ignored)
	bit14	n/a (ignored)
msb	bit15	n/a (ignored)
*/
