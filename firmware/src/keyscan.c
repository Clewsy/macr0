#include "keyscan.h"



void keyscan_init(void)
{
	// Set rows as outputs and initialise all as high (disabled).
	KEYS_DDR |= ((1 << ROW_1) | (1 << ROW_2));
	KEYS_PORT |= ((1 << ROW_1) | (1 << ROW_2));

	// Set columns as inputs and enable pull-ups.
	KEYS_DDR &= ~((1 << COL_1) | (1 << COL_2));
	KEYS_PORT |= ((1 << COL_1) | (1 << COL_2));
}


//#define NUM_ROWS 2
//#define NUM_COLS 2
//const char KEYMAP[NUM_ROWS][NUM_COLS] PROGMEM = {
//	{'e', 'f'},
//	{'g', 'h'}
//};



char keyscan_get_keys(void)
{
	uint8_t col_array[2] = {COL_1, COL_2};
	uint8_t row_array[2] = {ROW_1, ROW_2};

	for(uint8_t r = 0; r < 2; r++)
	{
		KEYS_PORT &= ~(1 << row_array[r]);	// Set low current row (enable check).

		while(!(~KEYS_PINS & (1 << row_array[r]))) {}	// Wait until row is set low before continueing, otherwise column checks can be missed.

		for(uint8_t c = 0; c < 2; c++)
		{
			if(~KEYS_PINS & (1 << col_array[c]))
			{
				KEYS_PORT |= (1 << row_array[r]);	// Set high current row (disable check).
				return(pgm_read_byte(&KEYMAP[r][c]));
//				return('a' + (2*r) + c);
			}
		}

		KEYS_PORT |= (1 << row_array[r]);	// Set high current row (disable check).
	}

	return(NO_KEY);
}

//This function takes a character from the string and converts it to the value corresponding to the appropriate keystroke.
//Refer to LUFA driver file LUFA/USB/CLASS/COMMON/HIDClassCommon.h
uint8_t keyscan_char_to_key(char c)
{
	switch ((uint8_t)c)	//Typecast the character as an integer.
	{
		case 'A' ... 'Z' :	return ((uint8_t)c - 61);
		case 'a' ... 'z' :	return ((uint8_t)c - 93);
		case '1' ... '9' :	return ((uint8_t)c - 19);
		case '0' :		return (HID_KEYBOARD_SC_0_AND_CLOSING_PARENTHESIS);
		case '!' :		return (HID_KEYBOARD_SC_1_AND_EXCLAMATION);
		case '@' :		return (HID_KEYBOARD_SC_2_AND_AT);
		case '#' :		return (HID_KEYBOARD_SC_3_AND_HASHMARK);
		case '$' :		return (HID_KEYBOARD_SC_4_AND_DOLLAR);
		case '%' :		return (HID_KEYBOARD_SC_5_AND_PERCENTAGE);
		case '^' :		return (HID_KEYBOARD_SC_6_AND_CARET);
		case '&' :		return (HID_KEYBOARD_SC_7_AND_AMPERSAND);
		case '*' :		return (HID_KEYBOARD_SC_8_AND_ASTERISK);
		case '(' :		return (HID_KEYBOARD_SC_9_AND_OPENING_PARENTHESIS);
		case ')' :		return (HID_KEYBOARD_SC_0_AND_CLOSING_PARENTHESIS);
		case '\n':		return (HID_KEYBOARD_SC_ENTER);
		case '\e':		return (HID_KEYBOARD_SC_ESCAPE);
		case '\b':		return (HID_KEYBOARD_SC_BACKSPACE);
		case '\t':		return (HID_KEYBOARD_SC_TAB);
		case ' ' :		return (HID_KEYBOARD_SC_SPACE);
		case '-' :
		case '_' :		return (HID_KEYBOARD_SC_MINUS_AND_UNDERSCORE);
		case '=' :
		case '+' :		return (HID_KEYBOARD_SC_EQUAL_AND_PLUS);
		case '[' :
		case '{' :		return (HID_KEYBOARD_SC_OPENING_BRACKET_AND_OPENING_BRACE);
		case ']' :
		case '}' :		return (HID_KEYBOARD_SC_CLOSING_BRACKET_AND_CLOSING_BRACE);
		case '\\':
		case '|' :		return (HID_KEYBOARD_SC_BACKSLASH_AND_PIPE);
		case ';' :
		case ':' :		return (HID_KEYBOARD_SC_SEMICOLON_AND_COLON);
		case '\'':
		case '"' :		return (HID_KEYBOARD_SC_APOSTROPHE_AND_QUOTE);
		case '`' :
		case '~' :		return (HID_KEYBOARD_SC_GRAVE_ACCENT_AND_TILDE);
		case ',' :
		case '<' :		return (HID_KEYBOARD_SC_COMMA_AND_LESS_THAN_SIGN);
		case '.' :
		case '>' :		return (HID_KEYBOARD_SC_DOT_AND_GREATER_THAN_SIGN);
		case '/' :
		case '?' :		return (HID_KEYBOARD_SC_SLASH_AND_QUESTION_MARK);

		default :		return NO_KEY;	//If there is no character match "fail" by sending a NO_KEY code.
	}
}
