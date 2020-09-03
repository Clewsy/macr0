// This version of Keyboard.h has been modified from the original demo in Dean Camera's LUFA repository.
// Significant changes and additions noted in comments prepended with "clewsy".

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


#ifndef _KEYBOARD_H_
#define _KEYBOARD_H_

	// Includes:
	#include <avr/io.h>
	#include <avr/wdt.h>
	#include <avr/power.h>
	#include <avr/interrupt.h>
	#include <stdbool.h>
	#include <string.h>

	#include "Descriptors.h"

	#include <LUFA/Drivers/USB/USB.h>
	#include <LUFA/Drivers/Board/Joystick.h>
	#include <LUFA/Drivers/Board/Buttons.h>
	#include <LUFA/Drivers/Board/LEDs.h>
	#include <LUFA/Platform/Platform.h>

	// clewsy: keyscan.h and .c files written for specific use-case, custom board.
	#include "keyscan.h"

	// Type Defines:
	// clewsy: Type define for a Media Control HID report. This report contains the bits to match the usages defined
	// in the HID report of the device.  When set to a true, the relevant media controls on the host will be
	// triggered.  Note, the equivalent struct for keyboard is defined in the lufa library HIDClassCommon.h file.
	typedef struct
	{
		unsigned Play           : 1;
		unsigned Pause          : 1;
		unsigned FForward       : 1;
		unsigned Rewind         : 1;
		unsigned NextTrack      : 1;
		unsigned PreviousTrack  : 1;
		unsigned Stop           : 1;
		unsigned PlayPause      : 1;
		unsigned Mute           : 1;
		unsigned VolumeUp       : 1;
		unsigned VolumeDown     : 1;
		unsigned RESERVED       : 5;
	} ATTR_PACKED USB_MediaControllerReport_Data_t;

	// Function Prototypes:
	void SetupHIDHardware(void);
	void HID_Task(void);

	void EVENT_USB_Device_Connect(void);
	void EVENT_USB_Device_Disconnect(void);
	void EVENT_USB_Device_ConfigurationChanged(void);
	void EVENT_USB_Device_ControlRequest(void);
	void EVENT_USB_Device_StartOfFrame(void);

	void CreateKeyboardReport(USB_KeyboardReport_Data_t* const ReportData);
	void CreateMediaControllerReport(USB_MediaControllerReport_Data_t* const MediaReportData);
	void ProcessLEDReport(const uint8_t LEDReport);
	void SendNextKeyboardReport(void);
	void ReceiveNextKeyboardReport(void);
	void SendNextMediaControllerReport(void);

#endif

