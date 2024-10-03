/*
*	Config.h, Copyright Jonathan Mackey 2024
*
*	GNU license:
*	This program is free software: you can redistribute it and/or modify
*	it under the terms of the GNU General Public License as published by
*	the Free Software Foundation, either version 3 of the License, or
*	(at your option) any later version.
*
*	This program is distributed in the hope that it will be useful,
*	but WITHOUT ANY WARRANTY; without even the implied warranty of
*	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
*	GNU General Public License for more details.
*
*	You should have received a copy of the GNU General Public License
*	along with this program.  If not, see <http://www.gnu.org/licenses/>.
*
*	Please maintain this license information along with authorship and copyright
*	notices in any redistribution of this code.
*
*/
#ifndef Config_h
#define Config_h

#include <inttypes.h>
#include "PlatformDefs.h"

// This is coded for a STM32F103R

#define BAUD_RATE		19200
#define DEBOUNCE_DELAY	20		// ms, for buttons

#define DISPLAY_CONTROLLER	TFT_ILI9488
namespace Config
{
	const pin_t		kLEDA0Pin			= PA0;
	const pin_t		kErrorPin			= PA0;
	const pin_t		kLEDA1Pin			= PA1;
	const pin_t		kSteppersEnabledPin	= PA1;
	const pin_t		kTxPin				= PA2;
	const pin_t		kRxPin				= PA3;
	const pin_t		kSDDetectPin		= PA4;	// Ext Int 4
	const pin_t		kSCK				= PA5;
	const pin_t		kMISO				= PA6;
	const pin_t		kMOSI				= PA7;
	const pin_t		kZDirPin			= PA8;
	const pin_t		kEmergencyStopPin	= PA9;	// Ext Int 9
	const pin_t		kCancelBtnPin		= PA10;	// Ext Int 10
	const pin_t		kEnterBtnPin		= PA11;	// Ext Int 11
	const pin_t		kPDataPin			= PA12;
	const pin_t		kSWDIOPin			= PA13;
	const pin_t		kSWCLKPin			= PA14;
	const pin_t		kYStepPin			= PA15;	// PWM TIM2 CH1
	
	const pin_t		kDispResetPin		= PB0;
	const pin_t		kDispCSPin			= PB1;
	const pin_t		kBoot1Pin			= PB2;	// Externally pulled low
	const pin_t		kEXDirPin			= PB3;
	const pin_t		kEXStepPin			= PB4;	// PWM TIM3 CH1
	const pin_t		kEXEnablePin		= PB5;
	const pin_t		kZMinEndstopPin		= PB6;	// Ext Int 6
	const pin_t		kZMaxEndstopPin		= PB7;	// Ext Int 7
	const pin_t		kXStepPin			= PB8;	// PWM TIM4 CH3
	const pin_t		kXDirPin			= PB9;
	const pin_t		kSCLPin				= PB10;
	const pin_t		kSDAPin				= PB11;
	const pin_t		kXFaultPin			= PB12;	// Ext Int 12
	const pin_t		kYFaultPin			= PB13;	// Ext Int 13
	const pin_t		kZFaultPin			= PB14;	// Ext Int 14
	const pin_t		kDispDCPin			= PB15;
	
	const pin_t		kXMinEndstopPin		= PC0;	// Ext Int 0
	const pin_t		kXMaxEndstopPin		= PC1;	// Ext Int 1
	const pin_t		kYMinEndstopPin		= PC2;	// Ext Int 2
	const pin_t		kYMaxEndstopPin		= PC3;	// Ext Int 3
	const pin_t		kTouchCSPin			= PC4;
	const pin_t		kTouchIRQPin		= PC5;	// Ext Int 5
	const pin_t		kBacklightPin		= PC6;
	const pin_t		kSDSelectPin		= PC7;
	const pin_t		kZEnablePin			= PC8;
	const pin_t		kZStepPin			= PC9;	// PWM TIM8 CH4
	const pin_t		kYEnablePin			= PC10;
	const pin_t		kYDirPin			= PC11;
	const pin_t		kXEnablePin			= PC12;
	const pin_t		kRTCOutPin			= PC13;	// To check load cap values
	const pin_t		kOsc32InPin			= PC14;	// RTC OSC
	const pin_t		kOsc32OutPin		= PC15;	// RTC OSC

	const pin_t		kMotorEnablePin		= PD2;

	const uint8_t	kDisplayRotation	= 3;	// 270
	
	const uint32_t	kXMinEndstopMask 	= _BV(0); //digitalPinToBitMask(PC0);
	const uint32_t	kXMaxEndstopMask 	= _BV(1); //digitalPinToBitMask(PC1);
	const uint32_t	kXEndstopMask		= kXMinEndstopMask + kXMaxEndstopMask;
	const uint32_t	kYMinEndstopMask 	= _BV(2); //digitalPinToBitMask(PC2);
	const uint32_t	kYMaxEndstopMask 	= _BV(3); //digitalPinToBitMask(PC3);
	const uint32_t	kYEndstopMask		= kYMinEndstopMask + kYMaxEndstopMask;
	const uint32_t	kZMinEndstopMask 	= _BV(6); //digitalPinToBitMask(PB6);
	const uint32_t	kZMaxEndstopMask 	= _BV(7); //digitalPinToBitMask(PB7);
	const uint32_t	kZEndstopMask		= kZMinEndstopMask + kZMaxEndstopMask;

	const uint32_t	kEmergencyStopMask	= _BV(9); //digitalPinToBitMask(PA9);
	const uint32_t	kCancelBtnMask 		= _BV(10); //digitalPinToBitMask(PA10);
	const uint32_t	kEnterBtnMask		= _BV(11); //digitalPinToBitMask(PA11);
	
	const uint32_t	kPINABtnMask = (kEnterBtnMask | kCancelBtnMask);

	/*
	*	MICROSTEPS can be set to 1, 2, 4, 8, 16, or 32
	*	Using MICROSTEPS 32 causes the steppers to run very quietly.
	*
	*	PCB dip switch settings:	1	=	000
	*								2	=	001
	*								4	=	010
	*								8	=	011
	*								16	=	100
	*								32	=	101, 110, 111
	*/
	#define	MICROSTEPS	32
	#if MICROSTEPS != 1
	#define TO_STEPS(steps) (steps/MICROSTEPS)
	#define TO_MICROSTEPS(steps) (steps*MICROSTEPS)
	#else
	#define TO_STEPS(steps) (steps)
	#define TO_MICROSTEPS(steps) (steps)
	#endif

	const uint8_t	kTextInset			= 3; // Makes room for drawing the selection frame
	const uint8_t	kTextVOffset		= 6; // Makes room for drawing the selection frame
	// To make room for the selection frame the actual font height in the font
	// file is reduced.  The actual height is kFontHeight.
	const uint8_t	kFontHeight			= 43;

#if 1	
	// Touchscreen min/max for 4" ILI9488 display
	const uint16_t	kDisplayWidth		= 320;
	const uint16_t	kDisplayHeight		= 480;
	const bool		kInvertTouchX		= false;
#else
	// Touchscreen min/max for 3.5" ILI9488 display
	const uint16_t	kDisplayWidth		= 320;
	const uint16_t	kDisplayHeight		= 480;
	const bool		kInvertTouchX		= true;
#endif

	const uint8_t kAT24CDeviceAddr = 0x50;	// Serial EEPROM
	const uint8_t kAT24CDeviceCapacity = 8;	// Value at end of AT24Cxxx xxx/8
	
	const uint8_t kMCP45X1DeviceAddr = 0x2E;	// I2C Digital POT
	
	/*
	*	The SKMPreferences occupy the first 12 bytes of the AT24C64 EEPROM
	*	The AT24C64 EEPROM has 8KB of storage.
	*/
	const uint16_t	kSKMPreferencesAddr		= 0;		// EEPROM Page 0

	struct SKMPreferences
	{
		uint8_t		unused[2];
		uint8_t		clockFormat;	// The time format (0=24, 1=12)
		uint8_t		unused2;
		uint16_t	tsMinMax[4];	// Touch screen alignment values
	}; // __attribute__ ((packed)); (was getting compiler warnings)
	
	const uint16_t	kKeyHolderDialogAddr		= 64;	// EEPROM Page 2
	struct SKeyHolderDialogPrefs
	{
		// Prefs for Setup holder origin dialog
		uint32_t	keyHolderOriginX;	// Steps to left edge of key holder from home
		uint32_t	keyHolderOriginZ;	// Steps to bottom of key holder slot from home
		uint32_t	keyHolderZeroOffsetX;	// This is a dialog preference only
		uint32_t	keyHolderZeroOffsetZ;	// This is a dialog preference only
		uint16_t	incMenuItemTag;	// Increment menu pref
	};
	
	/*
	*	To avoid cutting into the key holder, its depth plus a safe buffer is
	*	defined below as kKeyHolderRoughXMaxSteps.  This is the depth of the key
	*	holder slot.
	*/
	const uint32_t	kKeyHolderDepth = 457;	// = 4.57mm = 0.180 inches
	
	const int32_t	kKeyHolderRoughXSteps = 4700; // Initial X. Should stop ~0.5mm from key shoulder
	const int32_t	kKeyHolderRoughZSteps = 1600;	// Initial Z.  Should stop ~1.5mm above sample key 
	const int32_t	kKeyHolderRoughXMaxSteps = 4600; // Approx when cutter will hit key shoulder
	const int32_t	kKeyHolderRoughZMaxSteps = 2140; // Approx when cutter will hit holder
	const int32_t	kStepsPerMM = 100;

	const uint16_t	kCutKeyDialogAddr		= 128;	// EEPROM Page 4
	struct SCutKeyDialogPrefs
	{
		// Settings for the last key cut.
		// Used as initial values for the next key.
		uint16_t	keywayMenuItemTag;
		uint16_t	pinCountMenuItemTag;
		int32_t		pinDepths[8];
	};
}

#endif // Config_h

