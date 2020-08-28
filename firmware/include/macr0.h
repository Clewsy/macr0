/*
             LUFA Library
     Copyright (C) Dean Camera, 2018.

  dean [at] fourwalledcubicle [dot] com
           www.lufa-lib.org
*/

/*
  Copyright 2018  Dean Camera (dean [at] fourwalledcubicle [dot] com)

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
 *  Header file for Keyboard.c.
 */

#define __JOYSTICK_H__	//Set flag so the LUFA library doesn't search for the joystick header file.
#define __LEDS_H__	//Set flag so the LUFA library doesn't search for the leds header file.
#define __BUTTONS_H__	//Set flag so the LUFA library doesn't search for the buttons header file.

#ifndef _KEYBOARD_H_
#define _KEYBOARD_H_

	/* Includes: */
		#include <avr/io.h>
		#include <avr/wdt.h>
		#include <avr/power.h>
		#include <avr/interrupt.h>
		#include <stdbool.h>
		#include <string.h>


		#include <LUFA/Drivers/Board/Joystick.h>
		#include <LUFA/Drivers/Board/LEDs.h>
		#include <LUFA/Drivers/Board/Buttons.h>
		#include <LUFA/Drivers/USB/USB.h>
		#include <LUFA/Platform/Platform.h>

		#include "Descriptors.h"
		#include "The_String.h"
#include "cat4104.h"
#include "dimmer.h"

	/* Macros: */
		/** LED mask for the library LED driver, to indicate that the USB interface is not ready. */
		#define LEDMASK_USB_NOTREADY		(LED_ORANGE | LED_GREEN)

		/** LED mask for the library LED driver, to indicate that the USB interface is enumerating. */
		#define LEDMASK_USB_ENUMERATING		LED_ORANGE

		/** LED mask for the library LED driver, to indicate that the USB interface is ready. */
		#define LEDMASK_USB_READY		LED_GREEN

		/** LED mask for the library LED driver, to indicate that an error has occurred in the USB interface. */
		#define LEDMASK_USB_ERROR		0

		//Added to indicate status running the keystrokes for the string.
		#define LEDMASK_RUNNING			(LED_ORANGE | LED_GREEN)
		#define LEDMASK_DONE			0

		#define STARTUP_DELAY_MS	1000	//Startup delay in ms after enumerating and before beginning keystroke entry.
		#define REPEAT			2	//Number of times string is entered.
		#define KEY_DELAY_MS		4	//Delay in ms between keystrokes to ensure none are missed.
		#define FALSE			0	//Used with flags.
		#define TRUE			1	//Used with flags.
		#define NO_KEY			0x00	//Used to indicate a no-key keystroke event.
		#define NO_MODIFIER		0x00	//Used to indicate a no-modifier keystroke event.

	/* Function Prototypes: */
		void SetupHardware(void);

		void EVENT_USB_Device_Connect(void);
		void EVENT_USB_Device_Disconnect(void);
		void EVENT_USB_Device_ConfigurationChanged(void);
		void EVENT_USB_Device_ControlRequest(void);
		void EVENT_USB_Device_StartOfFrame(void);

		bool CALLBACK_HID_Device_CreateHIDReport(USB_ClassInfo_HID_Device_t* const HIDInterfaceInfo,
		                                         uint8_t* const ReportID,
		                                         const uint8_t ReportType,
		                                         void* ReportData,
		                                         uint16_t* const ReportSize);
		void CALLBACK_HID_Device_ProcessHIDReport(USB_ClassInfo_HID_Device_t* const HIDInterfaceInfo,
		                                          const uint8_t ReportID,
		                                          const uint8_t ReportType,
		                                          const void* ReportData,
		                                          const uint16_t ReportSize);

							  void SendKey(uint8_t k, uint8_t m);	//Sends a keystroke, calls the LUFA routines.
							  uint8_t CharToKey(char c);		//converts string char to keystroke value.
							  uint8_t CheckShift(char c);		//Determines if shift needed to turn keystroke into char.
#endif
