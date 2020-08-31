// This version of Keyboard.c has been modified from the original demo in Dean Camera's LUFA repository.
// Effectively I have used combinations of my own functions in addition to functions from the following .c files:
// - Keyboard.c at https://github.com/abcminiuser/lufa/tree/790ac4d125c1c5d72016b52c2b9ad58f5e5f8c62/Demos/Device/LowLevel/Keyboard
// - KeyboardMouse.c at https://github.com/abcminiuser/lufa/tree/790ac4d125c1c5d72016b52c2b9ad58f5e5f8c62/Demos/Device/LowLevel/KeyboardMouse
// - MediaController.c at https://github.com/abcminiuser/lufa/tree/790ac4d125c1c5d72016b52c2b9ad58f5e5f8c62/Projects/MediaController
//
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

#include "Keyboard.h"

// Indicates what report mode the host has requested, true for normal HID reporting mode, false for special boot protocol reporting mode.
static bool UsingReportProtocol = true;

// Current Idle period. This is set by the host via a Set Idle HID class request to silence the device's reports for either the entire idle duration,
// or until the report status changes (e.g. the user presses a key).
static uint16_t IdleCount = 500;

// Current Idle period remaining. When the IdleCount value is set, this tracks the remaining number of idle milliseconds.
// This is separate to the IdleCount timer and is incremented and compared as the host may request the current idle period via a Get Idle HID class request,
// thus its value must be preserved.
static uint16_t IdleMSRemaining = 0;

////////////////////////////////////////////////////////////////////////////////////////////////////////////
// clewsy: main function taken out of Keyboard.c and is instead in macr0.c
//int main(void)
//{
//	SetupHardware();
//
//	LEDs_SetAllLEDs(LEDMASK_USB_NOTREADY);
//	GlobalInterruptEnable();
//
//	for (;;)
//	{
//		HID_Task();
//		USB_USBTask();
//	}
//}

// Configures the board hardware and chip peripherals.
// clewsy: Use case is specifically an ATmega32U4 (ARCH_AVR8) so requirements for other architectures can be removed.
void SetupHardware(void)
{
	// Disable watchdog if enabled by bootloader/fuses.
	MCUSR &= ~(1 << WDRF);
	wdt_disable();

	// Disable clock division.
	clock_prescale_set(clock_div_1);

	// Hardware Initialization
//	LEDs_Init();
	USB_Init();
//	Buttons_Init();
}

// Event handler for the USB_Connect event. This indicates that the device is enumerating via the status LEDs and starts the library USB task
// to begin the enumeration and USB management process.
void EVENT_USB_Device_Connect(void)
{
	// Indicate USB enumerating.
//	LEDs_SetAllLEDs(LEDMASK_USB_ENUMERATING);

	// Default to report protocol on connect.
	UsingReportProtocol = true;
}

// Event handler for the USB_Disconnect event. This indicates that the device is no longer connected to a host via the status LEDs.
void EVENT_USB_Device_Disconnect(void)
{
	// Indicate USB not ready.
//	LEDs_SetAllLEDs(LEDMASK_USB_NOTREADY);
}

// Event handler for the USB_ConfigurationChanged event. This is fired when the host sets the current configuration of the USB device after enumeration,
// and configures the keyboard device endpoints.
void EVENT_USB_Device_ConfigurationChanged(void)
{
	bool ConfigSuccess = true;

	// Setup HID Report Endpoints.
	ConfigSuccess &= Endpoint_ConfigureEndpoint(KEYBOARD_IN_EPADDR, EP_TYPE_INTERRUPT, HID_EPSIZE, 1);
	ConfigSuccess &= Endpoint_ConfigureEndpoint(KEYBOARD_OUT_EPADDR, EP_TYPE_INTERRUPT, HID_EPSIZE, 1);
	ConfigSuccess &= Endpoint_ConfigureEndpoint(MEDIACONTROLLER_IN_EPADDR, EP_TYPE_INTERRUPT, HID_EPSIZE, 1);

	// Turn on Start-of-Frame events for tracking HID report period expiry.
	USB_Device_EnableSOFEvents();

	// Indicate endpoint configuration success or failure.
//	LEDs_SetAllLEDs(ConfigSuccess ? LEDMASK_USB_READY : LEDMASK_USB_ERROR);
}

// Event handler for the USB_ControlRequest event. This is used to catch and process control requests sent to the device from the USB host before
// passing along unhandled control requests to the library for processing internally.
void EVENT_USB_Device_ControlRequest(void)
{
	// Handle HID Class specific requests.
	switch (USB_ControlRequest.bRequest)
	{
		case HID_REQ_GetReport:
			if (USB_ControlRequest.bmRequestType == (REQDIR_DEVICETOHOST | REQTYPE_CLASS | REQREC_INTERFACE))
			{
				USB_KeyboardReport_Data_t KeyboardReportData;

				// Create the next keyboard report for transmission to the host.
				CreateKeyboardReport(&KeyboardReportData);

				Endpoint_ClearSETUP();

				// Write the report data to the control endpoint.
				Endpoint_Write_Control_Stream_LE(&KeyboardReportData, sizeof(KeyboardReportData));
				Endpoint_ClearOUT();
			}
			break;

		case HID_REQ_SetReport:
			if (USB_ControlRequest.bmRequestType == (REQDIR_HOSTTODEVICE | REQTYPE_CLASS | REQREC_INTERFACE))
			{
				Endpoint_ClearSETUP();

				// Wait until the LED report has been sent by the host.
				while (!(Endpoint_IsOUTReceived()))
				{
					if (USB_DeviceState == DEVICE_STATE_Unattached)
					  return;
				}

				// Read in the LED report from the host.
				uint8_t LEDStatus = Endpoint_Read_8();

				Endpoint_ClearOUT();
				Endpoint_ClearStatusStage();

				// Process the incoming LED report.
				ProcessLEDReport(LEDStatus);
			}
			break;

		case HID_REQ_GetProtocol:
			if (USB_ControlRequest.bmRequestType == (REQDIR_DEVICETOHOST | REQTYPE_CLASS | REQREC_INTERFACE))
			{
				Endpoint_ClearSETUP();

				// Write the current protocol flag to the host.
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

				// Set or clear the flag depending on what the host indicates that the current Protocol should be.
				UsingReportProtocol = (USB_ControlRequest.wValue != 0);
			}
			break;

		case HID_REQ_SetIdle:
			if (USB_ControlRequest.bmRequestType == (REQDIR_HOSTTODEVICE | REQTYPE_CLASS | REQREC_INTERFACE))
			{
				Endpoint_ClearSETUP();
				Endpoint_ClearStatusStage();

				// Get idle period in MSB, IdleCount must be multiplied by 4 to get number of milliseconds.
				IdleCount = ((USB_ControlRequest.wValue & 0xFF00) >> 6);
			}
			break;

		case HID_REQ_GetIdle:
			if (USB_ControlRequest.bmRequestType == (REQDIR_DEVICETOHOST | REQTYPE_CLASS | REQREC_INTERFACE))
			{
				Endpoint_ClearSETUP();

				// Write the current idle duration to the host, must be divided by 4 before sent to host.
				Endpoint_Write_8(IdleCount >> 2);

				Endpoint_ClearIN();
				Endpoint_ClearStatusStage();
			}
			break;
	}
}

// Event handler for the USB device Start Of Frame event.
void EVENT_USB_Device_StartOfFrame(void)
{
	// One millisecond has elapsed, decrement the idle time remaining counter if it has not already elapsed.
	if (IdleMSRemaining)
	  IdleMSRemaining--;
}

// Fills the given HID report data structure with the next keyboard HID input report to send to the host.
// ReportData  Pointer to a HID report data structure to be filled.
// clewsy: create HID report function significantly changed.  Handed off to keyscan_getkeys() in keyscan.c
void CreateKeyboardReport(USB_KeyboardReport_Data_t* const ReportData)
{
	uint8_t UsedKeyCodes      = 0;

	// Clear the report contents.
	memset(ReportData, 0, sizeof(USB_KeyboardReport_Data_t));

//	/* Make sent key uppercase by indicating that the left shift key is pressed */
//	ReportData->Modifier = HID_KEYBOARD_MODIFIER_LEFTSHIFT;

	  ReportData->KeyCode[UsedKeyCodes++] = keyscan_get_keys();
_delay_ms(4);
//
//	if (JoyStatus_LCL & JOY_UP)
//	  ReportData->KeyCode[UsedKeyCodes++] = HID_KEYBOARD_SC_A;
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
////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////

// Fills the given HID report data structure with the next media controller HID input report to send to the host.
// MediaReportData  Pointer to a HID report data structure to be filled.
// clewsy: created this function from scratch, same concept as keyboard version.
void CreateMediaControllerReport(USB_MediaControllerReport_Data_t* const MediaReportData)
{

	// Clear the report contents.
	memset(MediaReportData, 0, sizeof(USB_MediaControllerReport_Data_t));

char kee = keyscan_get_keys();
if (kee == HID_KEYBOARD_SC_VOLUME_DOWN) MediaReportData->VolumeDown = true;
else if (kee == HID_KEYBOARD_SC_VOLUME_UP) MediaReportData->VolumeUp = true;

	/* Update the Media Control report with the user button presses */
	MediaReportData->Mute          = false;
	MediaReportData->PlayPause     = false;
//	MediaReportData->VolumeUp      = false;
//	MediaReportData->VolumeDown    = false;
	MediaReportData->PreviousTrack = false;
	MediaReportData->NextTrack     = false;

}
////////////////////////////////////////////////////////////////////////////////////////////////////////////


// Processes a received LED report, and updates the board LEDs states to match.
// LEDReport LED status report from the host.
// clewsy: Commented for now, may remove completely depending on how I decide to control LEDs on the custom board.
void ProcessLEDReport(const uint8_t LEDReport)
{
//	uint8_t LEDMask = LEDS_LED2;
//
//	if (LEDReport & HID_KEYBOARD_LED_NUMLOCK)
//	  LEDMask |= LEDS_LED1;
//
//	if (LEDReport & HID_KEYBOARD_LED_CAPSLOCK)
//	  LEDMask |= LEDS_LED3;
//
//	if (LEDReport & HID_KEYBOARD_LED_SCROLLLOCK)
//	  LEDMask |= LEDS_LED4;
//
//	// Set the status LEDs to the current Keyboard LED status.
//	LEDs_SetAllLEDs(LEDMask);
}

// Sends the next keyboard HID report to the host, via the keyboard data endpoint.
void SendNextKeyboardReport(void)
{
	static USB_KeyboardReport_Data_t PrevKeyboardReportData;
	USB_KeyboardReport_Data_t        KeyboardReportData;
	bool                             SendReport = false;

	// Create the next keyboard report for transmission to the host.
	CreateKeyboardReport(&KeyboardReportData);

	// Check if the idle period is set and has elapsed.
	if (IdleCount && (!(IdleMSRemaining)))
	{
		// Reset the idle time remaining counter.
		IdleMSRemaining = IdleCount;

		// Idle period is set and has elapsed, must send a report to the host.
		SendReport = true;
	}
	else
	{
		// Check to see if the report data has changed - if so a report MUST be sent.
		SendReport = (memcmp(&PrevKeyboardReportData, &KeyboardReportData, sizeof(USB_KeyboardReport_Data_t)) != 0);
	}

	// Select the Keyboard Report Endpoint.
	Endpoint_SelectEndpoint(KEYBOARD_IN_EPADDR);

	// Check if Keyboard Endpoint Ready for Read/Write and if we should send a new report.
	if (Endpoint_IsReadWriteAllowed() && SendReport)
	{
		// Save the current report data for later comparison to check for changes.
		PrevKeyboardReportData = KeyboardReportData;

		// Write Keyboard Report Data.
		Endpoint_Write_Stream_LE(&KeyboardReportData, sizeof(KeyboardReportData), NULL);

		// Finalize the stream transfer to send the last packet.
		Endpoint_ClearIN();
	}
}


// Sends the next media controller HID report to the host, via the keyboard data endpoint.
void SendNextMediaControllerReport(void)
{
	static USB_MediaControllerReport_Data_t	PrevMediaControllerReportData;
	USB_MediaControllerReport_Data_t	MediaControllerReportData;
	bool					SendReport = false;

	// Create the next keyboard report for transmission to the host.
	CreateMediaControllerReport(&MediaControllerReportData);

	// Check if the idle period is set and has elapsed.
	if (IdleCount && (!(IdleMSRemaining)))
	{
		// Reset the idle time remaining counter.
		IdleMSRemaining = IdleCount;

		// Idle period is set and has elapsed, must send a report to the host.
		SendReport = true;
	}
	else
	{
		// Check to see if the report data has changed - if so a report MUST be sent.
		SendReport = (memcmp(&PrevMediaControllerReportData, &MediaControllerReportData, sizeof(USB_MediaControllerReport_Data_t)) != 0);
	}

	// Select the Keyboard Report Endpoint.
	Endpoint_SelectEndpoint(MEDIACONTROLLER_IN_EPADDR);

	// Check if Keyboard Endpoint Ready for Read/Write and if we should send a new report.
	if (Endpoint_IsReadWriteAllowed() && SendReport)
	{
		// Save the current report data for later comparison to check for changes.
		PrevMediaControllerReportData = MediaControllerReportData;

		// Write Keyboard Report Data */
		Endpoint_Write_Stream_LE(&MediaControllerReportData, sizeof(MediaControllerReportData), NULL);

		// Finalize the stream transfer to send the last packet.
		Endpoint_ClearIN();
	}
}

// Reads the next LED status report from the host from the LED data endpoint, if one has been sent.
void ReceiveNextKeyboardReport(void)
{
	// Select the Keyboard LED Report Endpoint.
	Endpoint_SelectEndpoint(KEYBOARD_OUT_EPADDR);

	// Check if Keyboard LED Endpoint contains a packet.
	if (Endpoint_IsOUTReceived())
	{
		// Check to see if the packet contains data.
		if (Endpoint_IsReadWriteAllowed())
		{
			// Read in the LED report from the host.
			uint8_t LEDReport = Endpoint_Read_8();

			// Process the read LED report from the host.
			ProcessLEDReport(LEDReport);
		}

		// Handshake the OUT Endpoint - clear endpoint and ready for next report.
		Endpoint_ClearOUT();
	}
}

// Function to manage HID report generation and transmission to the host, when in report mode.
void HID_Task(void)
{
	// Device must be connected and configured for the task to run.
	if (USB_DeviceState != DEVICE_STATE_Configured)
	  return;

	// Send the next keypress report to the host.
	SendNextKeyboardReport();

	// Process the LED report sent from the host.
	ReceiveNextKeyboardReport();

	// Send the next media controller keypress report to the host.
	SendNextMediaControllerReport();

}

