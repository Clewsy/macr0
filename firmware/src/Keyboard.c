/*
             LUFA Library
     Copyright (C) Dean Camera, 2020.

  dean [at] fourwalledcubicle [dot] com
           www.lufa-lib.org
*/

/*
  Copyright 2020  Dean Camera (dean [at] fourwalledcubicle [dot] com)
  Copyright 2010  Denver Gingerich (denver [at] ossguy [dot] com)

  Permission to use, copy, modify, distribute, and sell this
  software and its documentation for any purpose is hereby granted
  without fee, provided that the above copyright notice appear in
  all copies and that both that the copyright notice and this
  permission notice and warranty disclaimer appear in supporting
  documentation, and that the name of the author not be used in
  advertising or publicity pertaining to distribution of the
  software without specific, written prior permission.

  The author disclaims all warranties with regard to this
  software, including all implied warranties of merchantability
  and fitness.  In no event shall the author be liable for any
  special, indirect or consequential damages or any damages
  whatsoever resulting from loss of use, data or profits, whether
  in an action of contract, negligence or other tortious action,
  arising out of or in connection with the use or performance of
  this software.
*/

/** \file
 *
 *  Main source file for the Keyboard demo. This file contains the main tasks of the demo and
 *  is responsible for the initial application hardware configuration.
 */

#include "Keyboard.h"

/** Indicates what report mode the host has requested, true for normal HID reporting mode, \c false for special boot
 *  protocol reporting mode.
 */
static bool UsingReportProtocol = true;

/** Current Idle period. This is set by the host via a Set Idle HID class request to silence the device's reports
 *  for either the entire idle duration, or until the report status changes (e.g. the user presses a key).
 */
static uint16_t IdleCount = 500;

/** Current Idle period remaining. When the IdleCount value is set, this tracks the remaining number of idle
 *  milliseconds. This is separate to the IdleCount timer and is incremented and compared as the host may request
 *  the current idle period via a Get Idle HID class request, thus its value must be preserved.
 */
static uint16_t IdleMSRemaining = 0;












#define KEYS_PORT	PORTD
#define KEYS_PINS	PIND
#define KEYS_DDR	DDRD
#define ROW_1		PD0
#define ROW_2		PD1
#define COL_1		PD2
#define COL_2		PD3
#define NO_KEY		0x00	//Used to indicate a no-key keystroke event.
#define NO_MODIFIER	0x00	//Used to indicate a no-modifier keystroke event.

void keys_init(void)
{
	// Set rows as outputs.
	KEYS_DDR |= ((1 << ROW_1) | (1 << ROW_2));

	// Set columns as inputs and enable pull-ups.
	KEYS_DDR &= ~((1 << COL_1) | (1 << COL_2));
	KEYS_PORT |= ((1 << COL_1) | (1 << COL_2));

//KEYS_PORT &= ~((1 << ROW_1) | (1 << ROW_2));
KEYS_PORT |= ((1 << ROW_1) | (1 << ROW_2));

}


/** Configures the board hardware and chip peripherals for the demo's functionality. */
void SetupHardware(void)
{
	/* Disable watchdog if enabled by bootloader/fuses */
	MCUSR &= ~(1 << WDRF);
	wdt_disable();

	/* Disable clock division */
	clock_prescale_set(clock_div_1);

keys_init();
	/* Hardware Initialization */
//	Joystick_Init();
//	LEDs_Init();
	USB_Init();
//	Buttons_Init();
}

/** Event handler for the USB_Connect event. This indicates that the device is enumerating via the status LEDs and
 *  starts the library USB task to begin the enumeration and USB management process.
 */
void EVENT_USB_Device_Connect(void)
{
	/* Indicate USB enumerating */
	LEDs_SetAllLEDs(LEDMASK_USB_ENUMERATING);

	/* Default to report protocol on connect */
	UsingReportProtocol = true;
}

/** Event handler for the USB_Disconnect event. This indicates that the device is no longer connected to a host via
 *  the status LEDs.
 */
void EVENT_USB_Device_Disconnect(void)
{
	/* Indicate USB not ready */
	LEDs_SetAllLEDs(LEDMASK_USB_NOTREADY);
}

/** Event handler for the USB_ConfigurationChanged event. This is fired when the host sets the current configuration
 *  of the USB device after enumeration, and configures the keyboard device endpoints.
 */
void EVENT_USB_Device_ConfigurationChanged(void)
{
	bool ConfigSuccess = true;

	/* Setup HID Report Endpoints */
	ConfigSuccess &= Endpoint_ConfigureEndpoint(KEYBOARD_IN_EPADDR, EP_TYPE_INTERRUPT, KEYBOARD_EPSIZE, 1);
	ConfigSuccess &= Endpoint_ConfigureEndpoint(KEYBOARD_OUT_EPADDR, EP_TYPE_INTERRUPT, KEYBOARD_EPSIZE, 1);

	/* Turn on Start-of-Frame events for tracking HID report period expiry */
	USB_Device_EnableSOFEvents();

	/* Indicate endpoint configuration success or failure */
	LEDs_SetAllLEDs(ConfigSuccess ? LEDMASK_USB_READY : LEDMASK_USB_ERROR);
}

/** Event handler for the USB_ControlRequest event. This is used to catch and process control requests sent to
 *  the device from the USB host before passing along unhandled control requests to the library for processing
 *  internally.
 */
void EVENT_USB_Device_ControlRequest(void)
{
	/* Handle HID Class specific requests */
	switch (USB_ControlRequest.bRequest)
	{
		case HID_REQ_GetReport:
			if (USB_ControlRequest.bmRequestType == (REQDIR_DEVICETOHOST | REQTYPE_CLASS | REQREC_INTERFACE))
			{
				USB_KeyboardReport_Data_t KeyboardReportData;

				/* Create the next keyboard report for transmission to the host */
				CreateKeyboardReport(&KeyboardReportData);

				Endpoint_ClearSETUP();

				/* Write the report data to the control endpoint */
				Endpoint_Write_Control_Stream_LE(&KeyboardReportData, sizeof(KeyboardReportData));
				Endpoint_ClearOUT();
			}

			break;
		case HID_REQ_SetReport:
			if (USB_ControlRequest.bmRequestType == (REQDIR_HOSTTODEVICE | REQTYPE_CLASS | REQREC_INTERFACE))
			{
				Endpoint_ClearSETUP();

				/* Wait until the LED report has been sent by the host */
				while (!(Endpoint_IsOUTReceived()))
				{
					if (USB_DeviceState == DEVICE_STATE_Unattached)
					  return;
				}

				/* Read in the LED report from the host */
				uint8_t LEDStatus = Endpoint_Read_8();

				Endpoint_ClearOUT();
				Endpoint_ClearStatusStage();

				/* Process the incoming LED report */
				ProcessLEDReport(LEDStatus);
			}

			break;
		case HID_REQ_GetProtocol:
			if (USB_ControlRequest.bmRequestType == (REQDIR_DEVICETOHOST | REQTYPE_CLASS | REQREC_INTERFACE))
			{
				Endpoint_ClearSETUP();

				/* Write the current protocol flag to the host */
				Endpoint_Write_8(UsingReportProtocol);

				Endpoint_ClearIN();
				Endpoint_ClearStatusStage();
			}

			break;
		case HID_REQ_SetProtocol:
			if (USB_ControlRequest.bmRequestType == (REQDIR_HOSTTODEVICE | REQTYPE_CLASS | REQREC_INTERFACE))
			{
				Endpoint_ClearSETUP();
				Endpoint_ClearStatusStage();

				/* Set or clear the flag depending on what the host indicates that the current Protocol should be */
				UsingReportProtocol = (USB_ControlRequest.wValue != 0);
			}

			break;
		case HID_REQ_SetIdle:
			if (USB_ControlRequest.bmRequestType == (REQDIR_HOSTTODEVICE | REQTYPE_CLASS | REQREC_INTERFACE))
			{
				Endpoint_ClearSETUP();
				Endpoint_ClearStatusStage();

				/* Get idle period in MSB, IdleCount must be multiplied by 4 to get number of milliseconds */
				IdleCount = ((USB_ControlRequest.wValue & 0xFF00) >> 6);
			}

			break;
		case HID_REQ_GetIdle:
			if (USB_ControlRequest.bmRequestType == (REQDIR_DEVICETOHOST | REQTYPE_CLASS | REQREC_INTERFACE))
			{
				Endpoint_ClearSETUP();

				/* Write the current idle duration to the host, must be divided by 4 before sent to host */
				Endpoint_Write_8(IdleCount >> 2);

				Endpoint_ClearIN();
				Endpoint_ClearStatusStage();
			}

			break;
	}
}

/** Event handler for the USB device Start Of Frame event. */
void EVENT_USB_Device_StartOfFrame(void)
{
	/* One millisecond has elapsed, decrement the idle time remaining counter if it has not already elapsed */
	if (IdleMSRemaining)
	  IdleMSRemaining--;
}





char get_key(void)
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
				return('a' + (2*r) + c);
			}
		}

		KEYS_PORT |= (1 << row_array[r]);	// Set high current row (disable check).
	}

	return(NO_KEY);
}
//This function takes a character from the string and converts it to the value corresponding to the appropriate keystroke.
//Refer to LUFA driver file LUFA/USB/CLASS/COMMON/HIDClassCommon.h
uint8_t CharToKey(char c)
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

/** Fills the given HID report data structure with the next HID report to send to the host.
 *
 *  \param[out] ReportData  Pointer to a HID report data structure to be filled
 */
void CreateKeyboardReport(USB_KeyboardReport_Data_t* const ReportData)
{
//	uint8_t JoyStatus_LCL     = Joystick_GetStatus();
//	uint8_t ButtonStatus_LCL  = Buttons_GetStatus();

	uint8_t UsedKeyCodes      = 0;

	/* Clear the report contents */
	memset(ReportData, 0, sizeof(USB_KeyboardReport_Data_t));

	/* Make sent key uppercase by indicating that the left shift key is pressed */
//	ReportData->Modifier = HID_KEYBOARD_MODIFIER_LEFTSHIFT;
//	ReportData->Modifier = NO_MODIFIER;



//	if (JoyStatus_LCL & JOY_UP)
	  ReportData->KeyCode[UsedKeyCodes++] = CharToKey(get_key());
_delay_ms(4);	//Dirty delay to prevent button bounce registering as a double-press.
//	else if (JoyStatus_LCL & JOY_DOWN)
//	  ReportData->KeyCode[UsedKeyCodes++] = HID_KEYBOARD_SC_B;
//
//	if (JoyStatus_LCL & JOY_LEFT)
//	  ReportData->KeyCode[UsedKeyCodes++] = HID_KEYBOARD_SC_C;
//	else if (JoyStatus_LCL & JOY_RIGHT)
//	  ReportData->KeyCode[UsedKeyCodes++] = HID_KEYBOARD_SC_D;
//
//	if (JoyStatus_LCL & JOY_PRESS)
//	  ReportData->KeyCode[UsedKeyCodes++] = HID_KEYBOARD_SC_E;
//
//	if (ButtonStatus_LCL & BUTTONS_BUTTON1)
//	  ReportData->KeyCode[UsedKeyCodes++] = HID_KEYBOARD_SC_F;
}

/** Processes a received LED report, and updates the board LEDs states to match.
 *
 *  \param[in] LEDReport  LED status report from the host
 */
void ProcessLEDReport(const uint8_t LEDReport)
{
	uint8_t LEDMask = LEDS_LED2;

	if (LEDReport & HID_KEYBOARD_LED_NUMLOCK)
	  LEDMask |= LEDS_LED1;

	if (LEDReport & HID_KEYBOARD_LED_CAPSLOCK)
	  LEDMask |= LEDS_LED3;

	if (LEDReport & HID_KEYBOARD_LED_SCROLLLOCK)
	  LEDMask |= LEDS_LED4;

	/* Set the status LEDs to the current Keyboard LED status */
	LEDs_SetAllLEDs(LEDMask);
}

/** Sends the next HID report to the host, via the keyboard data endpoint. */
void SendNextReport(void)
{
	static USB_KeyboardReport_Data_t PrevKeyboardReportData;
	USB_KeyboardReport_Data_t        KeyboardReportData;
	bool                             SendReport = false;

	/* Create the next keyboard report for transmission to the host */
	CreateKeyboardReport(&KeyboardReportData);

	/* Check if the idle period is set and has elapsed */
	if (IdleCount && (!(IdleMSRemaining)))
	{
		/* Reset the idle time remaining counter */
		IdleMSRemaining = IdleCount;

		/* Idle period is set and has elapsed, must send a report to the host */
		SendReport = true;
	}
	else
	{
		/* Check to see if the report data has changed - if so a report MUST be sent */
		SendReport = (memcmp(&PrevKeyboardReportData, &KeyboardReportData, sizeof(USB_KeyboardReport_Data_t)) != 0);
	}

	/* Select the Keyboard Report Endpoint */
	Endpoint_SelectEndpoint(KEYBOARD_IN_EPADDR);

	/* Check if Keyboard Endpoint Ready for Read/Write and if we should send a new report */
	if (Endpoint_IsReadWriteAllowed() && SendReport)
	{
		/* Save the current report data for later comparison to check for changes */
		PrevKeyboardReportData = KeyboardReportData;

		/* Write Keyboard Report Data */
		Endpoint_Write_Stream_LE(&KeyboardReportData, sizeof(KeyboardReportData), NULL);

		/* Finalize the stream transfer to send the last packet */
		Endpoint_ClearIN();
	}
}

/** Reads the next LED status report from the host from the LED data endpoint, if one has been sent. */
void ReceiveNextReport(void)
{
	/* Select the Keyboard LED Report Endpoint */
	Endpoint_SelectEndpoint(KEYBOARD_OUT_EPADDR);

	/* Check if Keyboard LED Endpoint contains a packet */
	if (Endpoint_IsOUTReceived())
	{
		/* Check to see if the packet contains data */
		if (Endpoint_IsReadWriteAllowed())
		{
			/* Read in the LED report from the host */
			uint8_t LEDReport = Endpoint_Read_8();

			/* Process the read LED report from the host */
			ProcessLEDReport(LEDReport);
		}

		/* Handshake the OUT Endpoint - clear endpoint and ready for next report */
		Endpoint_ClearOUT();
	}
}

/** Function to manage HID report generation and transmission to the host, when in report mode. */
void HID_Task(void)
{
	/* Device must be connected and configured for the task to run */
	if (USB_DeviceState != DEVICE_STATE_Configured)
	  return;

	/* Send the next keypress report to the host */
	SendNextReport();

	/* Process the LED report sent from the host */
	ReceiveNextReport();
}

