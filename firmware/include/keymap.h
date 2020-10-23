#include <avr/pgmspace.h>	// Required for writing to and reading from program memory space.

//#define NUM_KEYS 	4				// Number of regular keys.
//#define KEY_ARRAY	{KEY_1, KEY_2, KEY_3, KEY_4}	// An array of pins to scan through for key strokes.
//#define NUM_MACROS	0				// Number of macro keys.
//#define MACRO_ARRAY	{}				// An array of pins to scan through for macros.

#define KEY_1	PD0	// Pin connected to the first key.
#define KEY_2	PD1	// Pin connected to the second key.
#define KEY_3	PD2	// Pin connected to the third key.
#define KEY_4	PD3	// Pin connected to the fourth key.

#define MAX_PHYSICAL_KEYS 	4	// Maximum number of physical keys to scan.
#define MAX_MACRO_CHARS 	255	// Maximum number of sequential characters in a macro.

// Declare the key-to-pin, macro-to-pin, keymap and macromap arrays.
const uint8_t KEY_PIN_ARRAY[MAX_PHYSICAL_KEYS];			// Array defines which pins should be keystrokes.
const uint8_t MACRO_PIN_ARRAY[MAX_PHYSICAL_KEYS];		// Array defines which pins should be macros.
const char KEY_MAP[MAX_PHYSICAL_KEYS];				// Array maps key (pin) to a keyscan code.
const char MACRO_MAP[MAX_PHYSICAL_KEYS][MAX_MACRO_CHARS];	// Array maps key (pin) to a macro.

// Key scan-codes:
// Note these are defined in the lufa library file LUFA/Drivers/USB/Class/Common/HIDClassCommon.h but repeated again
// here for convenience.
#define HID_KEYBOARD_SC_RESERVED				0x00
#define HID_KEYBOARD_SC_ERROR_ROLLOVER				0x01
#define HID_KEYBOARD_SC_POST_FAIL				0x02
#define HID_KEYBOARD_SC_ERROR_UNDEFINED				0x03
#define HID_KEYBOARD_SC_A					0x04
#define HID_KEYBOARD_SC_B					0x05
#define HID_KEYBOARD_SC_C					0x06
#define HID_KEYBOARD_SC_D					0x07
#define HID_KEYBOARD_SC_E					0x08
#define HID_KEYBOARD_SC_F					0x09
#define HID_KEYBOARD_SC_G					0x0A
#define HID_KEYBOARD_SC_H					0x0B
#define HID_KEYBOARD_SC_I					0x0C
#define HID_KEYBOARD_SC_J					0x0D
#define HID_KEYBOARD_SC_K					0x0E
#define HID_KEYBOARD_SC_L					0x0F
#define HID_KEYBOARD_SC_M					0x10
#define HID_KEYBOARD_SC_N					0x11
#define HID_KEYBOARD_SC_O					0x12
#define HID_KEYBOARD_SC_P					0x13
#define HID_KEYBOARD_SC_Q					0x14
#define HID_KEYBOARD_SC_R					0x15
#define HID_KEYBOARD_SC_S					0x16
#define HID_KEYBOARD_SC_T					0x17
#define HID_KEYBOARD_SC_U					0x18
#define HID_KEYBOARD_SC_V					0x19
#define HID_KEYBOARD_SC_W					0x1A
#define HID_KEYBOARD_SC_X					0x1B
#define HID_KEYBOARD_SC_Y					0x1C
#define HID_KEYBOARD_SC_Z					0x1D
#define HID_KEYBOARD_SC_1_AND_EXCLAMATION			0x1E
#define HID_KEYBOARD_SC_2_AND_AT				0x1F
#define HID_KEYBOARD_SC_3_AND_HASHMARK				0x20
#define HID_KEYBOARD_SC_4_AND_DOLLAR				0x21
#define HID_KEYBOARD_SC_5_AND_PERCENTAGE			0x22
#define HID_KEYBOARD_SC_6_AND_CARET				0x23
#define HID_KEYBOARD_SC_7_AND_AMPERSAND				0x24
#define HID_KEYBOARD_SC_8_AND_ASTERISK				0x25
#define HID_KEYBOARD_SC_9_AND_OPENING_PARENTHESIS		0x26
#define HID_KEYBOARD_SC_0_AND_CLOSING_PARENTHESIS		0x27
#define HID_KEYBOARD_SC_ENTER					0x28
#define HID_KEYBOARD_SC_ESCAPE					0x29
#define HID_KEYBOARD_SC_BACKSPACE				0x2A
#define HID_KEYBOARD_SC_TAB					0x2B
#define HID_KEYBOARD_SC_SPACE					0x2C
#define HID_KEYBOARD_SC_MINUS_AND_UNDERSCORE			0x2D
#define HID_KEYBOARD_SC_EQUAL_AND_PLUS				0x2E
#define HID_KEYBOARD_SC_OPENING_BRACKET_AND_OPENING_BRACE	0x2F
#define HID_KEYBOARD_SC_CLOSING_BRACKET_AND_CLOSING_BRACE	0x30
#define HID_KEYBOARD_SC_BACKSLASH_AND_PIPE			0x31
#define HID_KEYBOARD_SC_NON_US_HASHMARK_AND_TILDE		0x32
#define HID_KEYBOARD_SC_SEMICOLON_AND_COLON			0x33
#define HID_KEYBOARD_SC_APOSTROPHE_AND_QUOTE			0x34
#define HID_KEYBOARD_SC_GRAVE_ACCENT_AND_TILDE			0x35
#define HID_KEYBOARD_SC_COMMA_AND_LESS_THAN_SIGN		0x36
#define HID_KEYBOARD_SC_DOT_AND_GREATER_THAN_SIGN		0x37
#define HID_KEYBOARD_SC_SLASH_AND_QUESTION_MARK			0x38
#define HID_KEYBOARD_SC_CAPS_LOCK				0x39
#define HID_KEYBOARD_SC_F1					0x3A
#define HID_KEYBOARD_SC_F2					0x3B
#define HID_KEYBOARD_SC_F3					0x3C
#define HID_KEYBOARD_SC_F4					0x3D
#define HID_KEYBOARD_SC_F5					0x3E
#define HID_KEYBOARD_SC_F6					0x3F
#define HID_KEYBOARD_SC_F7					0x40
#define HID_KEYBOARD_SC_F8					0x41
#define HID_KEYBOARD_SC_F9					0x42
#define HID_KEYBOARD_SC_F10					0x43
#define HID_KEYBOARD_SC_F11					0x44
#define HID_KEYBOARD_SC_F12					0x45
#define HID_KEYBOARD_SC_PRINT_SCREEN				0x46
#define HID_KEYBOARD_SC_SCROLL_LOCK				0x47
#define HID_KEYBOARD_SC_PAUSE					0x48
#define HID_KEYBOARD_SC_INSERT					0x49
#define HID_KEYBOARD_SC_HOME					0x4A
#define HID_KEYBOARD_SC_PAGE_UP					0x4B
#define HID_KEYBOARD_SC_DELETE					0x4C
#define HID_KEYBOARD_SC_END					0x4D
#define HID_KEYBOARD_SC_PAGE_DOWN				0x4E
#define HID_KEYBOARD_SC_RIGHT_ARROW				0x4F
#define HID_KEYBOARD_SC_LEFT_ARROW				0x50
#define HID_KEYBOARD_SC_DOWN_ARROW				0x51
#define HID_KEYBOARD_SC_UP_ARROW				0x52
#define HID_KEYBOARD_SC_NUM_LOCK				0x53
#define HID_KEYBOARD_SC_KEYPAD_SLASH				0x54
#define HID_KEYBOARD_SC_KEYPAD_ASTERISK				0x55
#define HID_KEYBOARD_SC_KEYPAD_MINUS				0x56
#define HID_KEYBOARD_SC_KEYPAD_PLUS				0x57
#define HID_KEYBOARD_SC_KEYPAD_ENTER				0x58
#define HID_KEYBOARD_SC_KEYPAD_1_AND_END			0x59
#define HID_KEYBOARD_SC_KEYPAD_2_AND_DOWN_ARROW			0x5A
#define HID_KEYBOARD_SC_KEYPAD_3_AND_PAGE_DOWN			0x5B
#define HID_KEYBOARD_SC_KEYPAD_4_AND_LEFT_ARROW			0x5C
#define HID_KEYBOARD_SC_KEYPAD_5				0x5D
#define HID_KEYBOARD_SC_KEYPAD_6_AND_RIGHT_ARROW		0x5E
#define HID_KEYBOARD_SC_KEYPAD_7_AND_HOME			0x5F
#define HID_KEYBOARD_SC_KEYPAD_8_AND_UP_ARROW			0x60
#define HID_KEYBOARD_SC_KEYPAD_9_AND_PAGE_UP			0x61
#define HID_KEYBOARD_SC_KEYPAD_0_AND_INSERT			0x62
#define HID_KEYBOARD_SC_KEYPAD_DOT_AND_DELETE			0x63
#define HID_KEYBOARD_SC_NON_US_BACKSLASH_AND_PIPE		0x64
#define HID_KEYBOARD_SC_APPLICATION				0x65

#define HID_KEYBOARD_SC_LEFT_CONTROL				0xE0
#define HID_KEYBOARD_SC_LEFT_SHIFT				0xE1
#define HID_KEYBOARD_SC_LEFT_ALT				0xE2
#define HID_KEYBOARD_SC_LEFT_GUI				0xE3
#define HID_KEYBOARD_SC_RIGHT_CONTROL				0xE4
#define HID_KEYBOARD_SC_RIGHT_SHIFT				0xE5
#define HID_KEYBOARD_SC_RIGHT_ALT				0xE6
#define HID_KEYBOARD_SC_RIGHT_GUI				0xE7

#define HID_MEDIACONTROLLER_SC_PLAY				0xF0
#define HID_MEDIACONTROLLER_SC_PAUSE				0xF1
#define HID_MEDIACONTROLLER_SC_FAST_FORWARD			0xF2
#define HID_MEDIACONTROLLER_SC_REWIND				0xF3
#define HID_MEDIACONTROLLER_SC_NEXT				0xF4
#define HID_MEDIACONTROLLER_SC_PREVIOUS				0xF5
#define HID_MEDIACONTROLLER_SC_STOP				0xF6
#define HID_MEDIACONTROLLER_SC_TOGGLE				0xF7
#define HID_MEDIACONTROLLER_SC_MUTE				0xF8
#define HID_MEDIACONTROLLER_SC_VOLUME_UP			0xF9
#define HID_MEDIACONTROLLER_SC_VOLUME_DOWN			0xFA
