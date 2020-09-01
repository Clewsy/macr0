#include "keyscan.h"

// Initialise the gpio for scanning rows and columns.
void keyscan_init(void)
{
	// Set rows as outputs and initialise all as high (disabled).
	KEYS_DDR |= ((1 << ROW_1) | (1 << ROW_2));
	KEYS_PORT |= ((1 << ROW_1) | (1 << ROW_2));

	// Set columns as inputs and enable pull-ups.
	KEYS_DDR &= ~((1 << COL_1) | (1 << COL_2));
	KEYS_PORT |= ((1 << COL_1) | (1 << COL_2));
}

// Parse the detected key and update the appropriate part of the report struct.
void handle_key(char key, keyscan_report_t *keyscan_report)
{
	// Media key scan values start at 0xF0, after the last keyboard modifier key scan.
	if(key > HID_KEYBOARD_SC_RIGHT_GUI)
	{
		// Convert the media key to a value from 0 to 10.
		key -= HID_MEDIACONTROLLER_SC_PLAY;

		// Shift a bit to the corresponding bit within the media_keys integer.
		keyscan_report->media_keys |= (1 << key);
	}

	// Modifier keys scan values start at 0xE0, after the last keyboard modifier key scan.
	else if(key > HID_KEYBOARD_SC_APPLICATION)
	{
		// Convert the media key to a value from 0 to 7.
		key -= HID_KEYBOARD_SC_LEFT_CONTROL;

		// Shift a bit to the corresponding bit within the modifier integer.
		keyscan_report->modifier |= (1 << key);		
	}

	// Regular keys scan values range from 0x00 to 0x65.
	else  if(key > HID_KEYBOARD_SC_RESERVED)
	{
		// Skip array elements that already have a keyscan value written.
		uint8_t i = 0;
		while((keyscan_report->keys[i]) && (i < MAX_KEYS)) i++;

		// Only register the key if the maximum number of simultaneous keys is not reached.
		if(i < MAX_KEYS) keyscan_report->keys[i] = key;
	}
}

void create_keyscan_report(keyscan_report_t *keyscan_report)
{
	// Start with a blank keyscan report.
	memset(keyscan_report, 0, sizeof(keyscan_report_t));

	// Define the pins to scan through.
	uint8_t row_array[2] = ROW_ARRAY;
	uint8_t col_array[2] = COL_ARRAY;

	// Loop through for each row.
	for(uint8_t r = 0; r < sizeof(row_array); r++)
	{
		// Set low current row (enable check).
		KEYS_PORT &= ~(1 << row_array[r]);

		// Wait until row is set low before continuing, otherwise column checks can be missed.
		while(!(~KEYS_PINS & (1 << row_array[r]))) {}

		// Loop through for each column in the current row.
		for(uint8_t c = 0; c < sizeof(col_array); c++)
		{
			// If the button in the current row and column is pressed, handle it.
			if(~KEYS_PINS & (1 << col_array[c]))
			{
				handle_key(pgm_read_byte(&KEYMAP[r][c]), keyscan_report);
			}
		}

		// Set high current row (disable check).
		KEYS_PORT |= (1 << row_array[r]);
	}

}
