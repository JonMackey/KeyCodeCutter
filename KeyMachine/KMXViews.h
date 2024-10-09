/*
*	KMXViews.h, Copyright Jonathan Mackey 2024
*	UI and constant strings.
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
#ifndef KMXViews_h
#define KMXViews_h

#include "ST77XXToXPT2046Alignment.h"
#include "ValueFormatter.h"
#include "XAlert.h"
#include "XAnimatedFontIcon.h"
#include "XCheckboxButton.h"
#include "XColoredView.h"
#include "XDateValueField.h"
#include "XDialogBox.h"
#include "XMenuButton.h"
#include "XMenuItem.h"
#include "XNumberValueField.h"
#include "XPopUpButton.h"
#include "XPushButton.h"
#include "XRadioButton.h"
#include "XRootView.h"
#include "XStepper.h"
#include "KMPinsValueField.h"

static const char kVerticalEllipsisStr[] = ".";
static const char kOKStr[] = "OK";
static const char kCancelStr[] = "Cancel";
static const char kCloseStr[] = "Close";

// Utilities dialog
static const char kMotorStr[] = "Motor:";
static const char kMotorSpeedStr[] = "Motor Speed:";
static const char kStartStr[] = "Start";
static const char kStopStr[] = "Stop";
static const char kSteppersStr[] = "Steppers:";
static const char kDisableStr[] = "Disable";
static const char kHomeStr[] = "Home";
static const char kSaveSettingsStr[] = "Settings to/from SD:";
static const char kSaveStr[] = "Save";
static const char kLoadStr[] = "Load";
static const char kSaveToSDFailedStr[] = "Save to SD failed.";
static const char kLoadFromSDFailedStr[] = "Load from SD failed.";
static const char kNoSDCardFoundStr[] = "No SD card found.";
static const char kSavedKMSettingsStr[] = "Saved to file KMSettings.txt";
static const char kLoadKMSettingsStr[] =	"Press OK to load file\n"
											"KMSettings.txt.  The board\n"
											"will restart if successful.";
static const char kSteppersBusyStr[] = "Steppers are currently busy.\n"
										"Wait for steppers to stop.";
static const char kKeyHolderOriginUndefinedStr[] = "Key holder origin not defined.\n"
										"Use Setup Origin to define.";
static const char kUnableToReadPrefsStr[] = "Unable to read preferences";

// Setup Key Holder Zero dialog (also used by adjust origin dialog)
static const char kIncrementStr[] = "Increment:";
static const char kUpStr[] = "Up";
static const char kLeftStr[] = "Left";
static const char kRightStr[] = "Right";
static const char kDownStr[] = "Down";
static const char kCurrentStr[] = "Current Pos";
static const char kXStr[] = "X:";
static const char kZStr[] = "Z:";
static const char kZeroOffsetStr[] = "Zero Offset";
static const char kSetStr[] = "Set";

// Adjust Origin dialog
static const char kAdjustOrginInstStr[] =	"X: + shifts all cuts right\n"
											"Z: + shifts all cuts down";
static const char kDeltaStr[] = "Delta";

// Cut Key dialog
static const char kKeywayStr[] = "Keyway:";
static const char kBowStr[] = "bow";
static const char kTipStr[] = "tip";
static const char kPinDepthStr[] = "Pin Depth:";
static const char kMMStr[] = "mm";
static const char kCutStr[] = "Cut";
static const char kResetStr[] = "Reset";
static const char kDepthErrMessageStr[] = "Cut too deep.";		// Should never happen
static const char kPinIndexMessageStr[] = "Pin Index error";	// Should never happen
static const char kMACSMessageStr[] =	"MACS Exceeded\n\n"
										"The maximum distance\n"
										"between cuts exceeded.";

// About Box
static const char kSoftwareNameStr[] = "Key Code Cutter";
static const char kVersionStr[] = "STM32 version 1.0";
static const char kCopyrightStr[] = "Copyright Jonathan Mackey 2024";

// Info view

// Touch screen alignment instructions.
static const char	kAlignmentInstMsg[] =
									"Precisely touch the tiny white dot\n"
									"within each circle. A magenta dot\n"
									"will reflect the alignment accuracy.\n"
									"The red circle at the top of the display\n"
									"will change to green when enough points\n"
									"have been entered to save. When satisfied\n"
									"with the alignment, press Enter to save.\n"
									"Press Cancel to quit alignment at any time.";


static const uint16_t	kInfoViewTag = 300;
static const uint16_t	kInfoDateValueFieldTag = 301;

static const uint16_t	kMainMenuBtnTag = 900;
static const uint16_t	kMainMenuTag = 901;

static const uint16_t	kAlertDialogTag = 600;
static const uint16_t	kWarningDialogTag = 700;
static const uint16_t	kKMStatusIconTag = 800;
static const uint16_t	kTouchScreenAlignmentTag = 1000;

static const uint16_t	kUtilitiesDialogTag = 1100;
//static const uint16_t	kLoadSettingsLabelTag = 1101;
static const uint16_t	kLoadSettingsBtnTag = 1102;
static const uint16_t	kSaveSettingsLabelTag = 1103;
static const uint16_t	kSaveSettingsBtnTag = 1104;
static const uint16_t	kSteppersDisableBtnTag = 1105;
static const uint16_t	kMotorStartStopBtnTag = 1106;
static const uint16_t	kMotorSpeedLabelTag = 1107;
static const uint16_t	kMotorSpeedValueFieldTag = 1108;
static const uint16_t	kMotorSpeedStepperTag = 1109;
static const uint16_t	kHomeSteppersBtnTag = 1110;
static const uint16_t	kSteppersLabelTag = 1111;
static const uint16_t	kDateValueFieldTag = 1112;
static const uint16_t	kDateValueStepperTag = 1113;
static const uint16_t	kSetTimeBtnTag = 1114;

static const uint16_t	kAboutBoxTag = 1200;
static const uint16_t	kSoftwareNameLabelTag = 1201;
static const uint16_t	kVersionLabelTag = 1202;
static const uint16_t	kCopyrightLabelTag = 1203;

static const uint16_t	kSetupKeyHolderDialogTag = 1300;
static const uint16_t	kIncrementLabelTag = 1302;
static const uint16_t	kIncrementPopUpTag = 1303;
static const uint16_t	kIncrementMenuTag = 1304;
static const uint16_t	kUpBtnTag = 1305;
static const uint16_t	kLeftBtnTag = 1306;
static const uint16_t	kRightBtnTag = 1307;
static const uint16_t	kDownBtnTag = 1308;
static const uint16_t	kCurrentLabelTag = 1309;
static const uint16_t	kCurrentXLabelTag = 1310;
static const uint16_t	kCurrentXValueFieldTag = 1311;
static const uint16_t	kCurrentZLabelTag = 1312;
static const uint16_t	kCurrentZValueFieldTag = 1313;
static const uint16_t	kZeroOffsetLabelTag = 1314;
static const uint16_t	kZeroXOffsetLabelTag = 1315;
static const uint16_t	kZeroXOffsetValueFieldTag = 1316;
static const uint16_t	kZeroXOffsetStepperTag = 1317;
static const uint16_t	kZeroZOffsetLabelTag = 1318;
static const uint16_t	kZeroZOffsetValueFieldTag = 1319;
static const uint16_t	kZeroZOffsetStepperTag = 1320;

static const uint16_t	kCutKeyDialogTag = 1400;
static const uint16_t	kKeywayLabelTag = 1401;
static const uint16_t	kKeywayPopUpTag = 1402;
static const uint16_t	kKeywayMenuTag = 1403;
static const uint16_t	kPinCountPopUpTag = 1404;
static const uint16_t	kPinCountMenuTag = 1405;
static const uint16_t	kBowLabelTag = 1406;
static const uint16_t	kTipLabelTag = 1407;
static const uint16_t	kPinsValueFieldTag = 1408;
static const uint16_t	kPinsValueStepperTag = 1409;
static const uint16_t	kPinDepthLabelTag = 1410;
static const uint16_t	kPinDepthValueFieldTag = 1411;
static const uint16_t	kPinValueStepperTag = 1412;
static const uint16_t	kMMLabelTag = 1413;
static const uint16_t	kResetBtnTag = 1414;

static const uint16_t	kAdjustOriginDialogTag = 1500;
static const uint16_t	kAdjustOrginInstLabelTag = 1501;
static const uint16_t	kDeltaLabelTag = 1502;
static const uint16_t	kDeltaXLabelTag = 1503;
static const uint16_t	kDeltaXValueFieldTag = 1504;
static const uint16_t	kDeltaXStepperTag = 1505;
static const uint16_t	kDeltaZLabelTag = 1506;
static const uint16_t	kDeltaZValueFieldTag = 1507;
static const uint16_t	kDeltaZStepperTag = 1508;
static const uint16_t	kDeltaMMLabelTag = 1509;

static const uint16_t	kSpaceBetween = 10;
static const uint16_t	kLabelYAdj = 4;
static const uint16_t	kRowHeight = 36;
static const uint16_t	kDialogBGColor = 0xF77D;

static const uint16_t	kInfoMenuItem = 1;
static const uint16_t	kCutKeyMenuItem = 2;
static const uint16_t	kSeparatorMenuItem = 3;
static const uint16_t	kUtilitiesMenuItem = 4;
static const uint16_t	kAdjustOriginMenuItem = 5;
static const uint16_t	kSetupZeroMenuItem = 6;
static const uint16_t	kAboutMenuItem = 7;

/*
*	The touch screen alignment view is displayed when the Enter button of the
*	5 button array is pressed.  This button array is only attached/needed when
*	alignment of the touch screen is performed.  The result of the alignment
*	is written to the EEPROM.
*/
ST77XXToXPT2046Alignment touchScreenAlignment(kTouchScreenAlignmentTag,
				&UI20ptFont, nullptr,
				kAlignmentInstMsg);
				
// The alertDialog is displayed within the filter settings dialog so it has
// the same FG and BG color.
XAlert	alertDialog(kAlertDialogTag, &touchScreenAlignment,
				kOKStr, kCancelStr, nullptr,
				&UI20ptFont,
				nullptr, kDialogBGColor, kDialogBGColor);

// The warning dialog is displayed on a black background
XAlert	warningDialog(kWarningDialogTag, &alertDialog,
				kOKStr, nullptr, nullptr,
				&UI20ptFont,
				nullptr, kDialogBGColor, kDialogBGColor);

//	Main menu
static const char kAboutStr[] = "About";
static const char kUtilitiesStr[] = "Utilities";
static const char kSetupZeroStr[] = "Setup Origin";
static const char kSeparatorStr[] = "-";
static const char kCutKeyStr[] = "Cut Key";
static const char kAdjustOriginStr[] = "Adjust Origin";
static const char kInfoStr[] = "Info";

XMenuItem	aboutMenuItem(kAboutMenuItem, kAboutStr);
XMenuItem	setupZeroMenuItem(kSetupZeroMenuItem, kSetupZeroStr, &aboutMenuItem);
XMenuItem	adjustOriginMenuItem(kAdjustOriginMenuItem, kAdjustOriginStr, &setupZeroMenuItem);
XMenuItem	utilitiesMenuItem(kUtilitiesMenuItem, kUtilitiesStr, &adjustOriginMenuItem);
XMenuItem	separatorMenuItem(kSeparatorMenuItem, kSeparatorStr, &utilitiesMenuItem);
XMenuItem	cutKeyMenuItem(kCutKeyMenuItem, kCutKeyStr, &separatorMenuItem);
XMenuItem	infoMenuItem(kInfoMenuItem, kInfoStr, &cutKeyMenuItem);
XMenu		mainMenu(kMainMenuTag, &UI20ptFont, &infoMenuItem);
XMenuButton mainMenuBtn(480-30, 0, 27, 0,
				kMainMenuBtnTag, &mainMenu, &warningDialog,
				kVerticalEllipsisStr, &UI20ptFont);

XAnimatedFontIcon kmStatusIcon(480-32-2, 320-32-2, 32, 32,
				kKMStatusIconTag, &mainMenuBtn, 'A', 'B', &KM_Icons::font);


//	Increment menu used in setup zero dialog
static const char k10mmStr[] = "10 mm";
static const char k1mmStr[] = "1 mm";
static const char kPt1mmStr[] = "0.1 mm";
static const char kPt01mmStr[] = "0.01 mm";

/*
*	The menu item tag values below double as mm values used to set the value
*	field increment.  e.g. 1000 = 10.00mm, 1 = 0.01mm
*/
static const uint16_t	k10mmMenuItem = 1000;
static const uint16_t	k1mmMenuItem = 100;
static const uint16_t	kPt1mmMenuItem = 10;
static const uint16_t	kPt01mmMenuItem = 1;
XMenuItem	pt01MenuItem(kPt01mmMenuItem, kPt01mmStr);
XMenuItem	pt1MenuItem(kPt1mmMenuItem, kPt1mmStr, &pt01MenuItem);
XMenuItem	oneMenuItem(k1mmMenuItem, k1mmStr, &pt1MenuItem);
XMenuItem	tenMenuItem(k10mmMenuItem, k10mmStr, &oneMenuItem);
XMenu		incrementMenu(kIncrementMenuTag,
				&UI20ptFont, &tenMenuItem, nullptr, kDialogBGColor);

// Setup zero dialog

static const uint16_t	kBtnXAdj = 43;	// Used to center the 4 buttons

XPushButton upBtn(90+kBtnXAdj, 0, 80, 0,
				kUpBtnTag, nullptr, kUpStr,
				&UI20ptFont,
				XFont::eWhite, kDialogBGColor);
XPushButton leftBtn(kBtnXAdj, 18, 80, 0,
				kLeftBtnTag, &upBtn, kLeftStr,
				&UI20ptFont,
				XFont::eWhite, kDialogBGColor);
XPushButton rightBtn(180+kBtnXAdj, 18, 80, 0,
				kRightBtnTag, &leftBtn, kRightStr,
				&UI20ptFont,
				XFont::eWhite, kDialogBGColor);
XPushButton downBtn(90+kBtnXAdj, kRowHeight, 80, 0,
				kDownBtnTag, &rightBtn, kDownStr,
				&UI20ptFont,
				XFont::eWhite, kDialogBGColor);
				
static const uint16_t	kGapYAdj = 10;	// Used to increase the gap between the
										// buttons and the views below

XLabel		currentLabel(0, kLabelYAdj + (kRowHeight*2) + kGapYAdj, 99, 26,
				kCurrentLabelTag, &downBtn, kCurrentStr,
				&UI20ptFont, nullptr,
				XFont::eBlack, kDialogBGColor, XFont::eAlignRight);
XLabel		currentXLabel(99+kSpaceBetween, kLabelYAdj + (kRowHeight*2) + kGapYAdj, 20, 26,
				kCurrentXLabelTag, &currentLabel, kXStr,
				&UI20ptFont, nullptr,
				XFont::eBlack, kDialogBGColor, XFont::eAlignRight);
XNumberValueField currentXValueField(125+kSpaceBetween, (kRowHeight*2) + kGapYAdj +3, 70,
				kCurrentXValueFieldTag, &currentXLabel,
				&UI20ptFont, 0, 10000, 0, 1, false, false,
				&ValueFormatter::Decimal22ToString,
				0xFB00, kDialogBGColor, XFont::eAlignLeft);
XLabel		currentZLabel(230, kLabelYAdj + (kRowHeight*2) + kGapYAdj, 20, 26,
				kCurrentZLabelTag, &currentXValueField, kZStr,
				&UI20ptFont, nullptr,
				XFont::eBlack, kDialogBGColor, XFont::eAlignRight);
XNumberValueField currentZValueField(245+kSpaceBetween, (kRowHeight*2) + kGapYAdj +3, 70,
				kCurrentZValueFieldTag, &currentZLabel,
				&UI20ptFont, 0, 10000, 0, 1, false, false,
				&ValueFormatter::Decimal22ToString,
				0xFB00, kDialogBGColor, XFont::eAlignLeft);

XLabel		zeroOffsetLabel(0, kLabelYAdj + (kRowHeight*3) + kGapYAdj, 99, 26,
				kZeroOffsetLabelTag, &currentZValueField, kZeroOffsetStr,
				&UI20ptFont, nullptr,
				XFont::eBlack, kDialogBGColor, XFont::eAlignRight);
XLabel		zeroXOffsetLabel(99+kSpaceBetween, kLabelYAdj + (kRowHeight*3) + kGapYAdj, 20, 26,
				kZeroXOffsetLabelTag, &zeroOffsetLabel, kXStr,
				&UI20ptFont, nullptr,
				XFont::eBlack, kDialogBGColor, XFont::eAlignRight);
XNumberValueField zeroXOffsetValueField(130+kSpaceBetween, (kRowHeight*3) + kGapYAdj, 70,
				kZeroXOffsetValueFieldTag, &zeroXOffsetLabel,
				&UI20ptFont, 0, 8500, 0, 1, false, false,
				&ValueFormatter::Decimal22ToString,
				0xFB00, kDialogBGColor, XFont::eAlignLeft);
XStepper	zeroXOffsetStepper(210, (kRowHeight*3)-2 + kGapYAdj, 0, 0,
				kZeroXOffsetStepperTag, &zeroXOffsetValueField);
XLabel		zeroZOffsetLabel(230, kLabelYAdj + (kRowHeight*3) + kGapYAdj, 20, 26,
				kZeroZOffsetLabelTag, &zeroXOffsetStepper, kZStr,
				&UI20ptFont, nullptr,
				XFont::eBlack, kDialogBGColor, XFont::eAlignRight);
XNumberValueField zeroZOffsetValueField(250+kSpaceBetween, (kRowHeight*3) + kGapYAdj, 70,
				kZeroZOffsetValueFieldTag, &zeroZOffsetLabel,
				&UI20ptFont, 0, 8500, 0, 1, false, false,
				&ValueFormatter::Decimal22ToString,
				0xFB00, kDialogBGColor, XFont::eAlignLeft);
XStepper	zeroZOffsetStepper(330, (kRowHeight*3)-2 + kGapYAdj, 0, 0,
				kZeroZOffsetStepperTag, &zeroZOffsetValueField);
				
XLabel		incrementLabel(30, kLabelYAdj+(kRowHeight*4) + kGapYAdj, 99, 26,
				kIncrementLabelTag, &zeroZOffsetStepper, kIncrementStr,
				&UI20ptFont, nullptr,
				XFont::eBlack, kDialogBGColor, XFont::eAlignRight);
XPopUpButton incrementPopUp(125+kSpaceBetween, (kRowHeight*4) + kGapYAdj, 105, 0,
				kIncrementPopUpTag, &incrementMenu, &incrementLabel,
				&UI20ptFont,
				XPopUpButton::eLargePopUpSize,
				XFont::eWhite, kDialogBGColor);

XDialogBox	setupKeyHolderOriginDialog(&incrementPopUp,
				kSetupKeyHolderDialogTag, &kmStatusIcon,
				kSetStr, kCancelStr, kSetupZeroStr,
				&UI20ptFont,
				nullptr, kDialogBGColor);

// Adjust origin dialog
XLabel		adjustOrginInstLabel(0, 0, 170, 52,
				kAdjustOrginInstLabelTag, nullptr, kAdjustOrginInstStr,
				&UI20ptFont, nullptr,
				XFont::eBlack, kDialogBGColor, XFont::eAlignLeft);
XLabel		deltaLabel(0, kLabelYAdj + (kRowHeight*2), 53, 26,
				kDeltaLabelTag, &adjustOrginInstLabel, kDeltaStr,
				&UI20ptFont, nullptr,
				XFont::eBlack, kDialogBGColor, XFont::eAlignLeft);
XLabel		deltaXLabel(43+kSpaceBetween, kLabelYAdj + (kRowHeight*2), 20, 26,
				kDeltaXLabelTag, &deltaLabel, kXStr,
				&UI20ptFont, nullptr,
				XFont::eBlack, kDialogBGColor, XFont::eAlignRight);
XNumberValueField deltaXValueField(74+kSpaceBetween, (kRowHeight*2), 40,
				kDeltaXValueFieldTag, &deltaXLabel,
				&UI20ptFont, 0, 100, -100, 1, false, false,
				&ValueFormatter::Decimal22ToString,
				0xFB00, kDialogBGColor, XFont::eAlignLeft);
XStepper	deltaXStepper(124, (kRowHeight*2)-2, 0, 0,
				kDeltaXStepperTag, &deltaXValueField);
XLabel		deltaZLabel(144, kLabelYAdj + (kRowHeight*2), 20, 26,
				kDeltaZLabelTag, &deltaXStepper, kZStr,
				&UI20ptFont, nullptr,
				XFont::eBlack, kDialogBGColor, XFont::eAlignRight);
XNumberValueField deltaZValueField(164+kSpaceBetween, (kRowHeight*2), 40,
				kDeltaZValueFieldTag, &deltaZLabel,
				&UI20ptFont, 0, 100, -100, 1, false, false,
				&ValueFormatter::Decimal22ToString,
				0xFB00, kDialogBGColor, XFont::eAlignLeft);
XStepper	deltaZStepper(214, (kRowHeight*2)-2, 0, 0,
				kDeltaZStepperTag, &deltaZValueField);
XLabel		deltaMMLabel(239, kLabelYAdj + (kRowHeight*2), 34, 26,
				kDeltaMMLabelTag, &deltaZStepper, kMMStr,
				&UI20ptFont, nullptr,
				XFont::eBlack, kDialogBGColor, XFont::eAlignRight);

XDialogBox	adjustOriginDialog(&deltaMMLabel,
				kAdjustOriginDialogTag, &setupKeyHolderOriginDialog,
				kSetStr, kCancelStr, kAdjustOriginStr,
				&UI20ptFont,
				nullptr, kDialogBGColor);

//	Pin count menu used in cut key dialog
static const char k6PinStr[] = "6 Pin";
static const char k5PinStr[] = "5 Pin";
static const char k4PinStr[] = "4 Pin";

static const uint16_t	k6PinMenuItem = 6;
static const uint16_t	k5PinMenuItem = 5;
static const uint16_t	k4PinMenuItem = 4;
XMenuItem	fourPinMenuItem(k4PinMenuItem, k4PinStr);
XMenuItem	fivePinMenuItem(k5PinMenuItem, k5PinStr, &fourPinMenuItem);
XMenuItem	sixPinMenuItem(k6PinMenuItem, k6PinStr, &fivePinMenuItem);
XMenu		pinCountMenu(kPinCountMenuTag,
				&UI20ptFont, &sixPinMenuItem, nullptr, kDialogBGColor);

//	Keyway menu used in cut key dialog
static const char kSchlageSC1Str[] = "Schlage SC1";
static const char kKwiksetKW1Str[] = "Kwikset KW1";

static const uint16_t	kSchlageSC1MenuItem = 2;
static const uint16_t	kKwiksetKW1MenuItem = 1;
XMenuItem	kwikseteMenuItem(kKwiksetKW1MenuItem, kKwiksetKW1Str);
XMenuItem	schlageMenuItem(kSchlageSC1MenuItem, kSchlageSC1Str, &kwikseteMenuItem);
XMenu		keywayMenu(kKeywayMenuTag,
				&UI20ptFont, &schlageMenuItem, nullptr, kDialogBGColor);

// Cut key dialog
XLabel		keywayLabel(0, kLabelYAdj, 87, 26,
				kKeywayLabelTag, nullptr, kKeywayStr,
				&UI20ptFont, nullptr,
				XFont::eBlack, kDialogBGColor, XFont::eAlignRight);
XPopUpButton keywayPopUp(87+kSpaceBetween, 0, 145, 0,
				kKeywayPopUpTag, &keywayMenu, &keywayLabel,
				&UI20ptFont,
				XPopUpButton::eLargePopUpSize,
				XFont::eWhite, kDialogBGColor);
XPopUpButton pinCountPopUp(87+kSpaceBetween+145+kSpaceBetween, 0, 75, 0,
				kPinCountPopUpTag, &pinCountMenu, &keywayPopUp,
				&UI20ptFont,
				XPopUpButton::eLargePopUpSize,
				XFont::eWhite, kDialogBGColor);
XLabel		bowLabel(0, kLabelYAdj + kRowHeight, 87, 26,
				kBowLabelTag, &pinCountPopUp, kBowStr,
				&UI20ptFont, nullptr,
				XFont::eBlack, kDialogBGColor, XFont::eAlignRight);
XLabel		tipLabel(87+kSpaceBetween+145+kSpaceBetween, kLabelYAdj + kRowHeight, 37, 26,
				kTipLabelTag, &bowLabel, kTipStr,
				&UI20ptFont, nullptr,
				XFont::eBlack, kDialogBGColor, XFont::eAlignLeft);
KMPinsValueField pinsValueField(0, kRowHeight, 0,
				kPinsValueFieldTag, &tipLabel,
				&UI20ptFont, XFont::eBlack, kDialogBGColor);
XStepper	pinsValueStepper(227, kRowHeight, 0, 0,
				kPinsValueStepperTag, &pinsValueField);
XLabel		pinDepthLabel(0, kLabelYAdj + (kRowHeight*2), 87, 26,
				kPinDepthLabelTag, &pinsValueStepper, kPinDepthStr,
				&UI20ptFont, nullptr,
				XFont::eBlack, kDialogBGColor, XFont::eAlignRight);
XNumberValueField pinDepthValueField(0, 0, 40,
				kPinDepthValueFieldTag, &pinDepthLabel,
				&UI20ptFont, 0, 8500, 0, 1, false, false,
				&ValueFormatter::Decimal22ToString,
				0xFB00, kDialogBGColor);
XStepper	pinValueStepper(134, kRowHeight*2, 0, 0,
				kPinValueStepperTag, &pinDepthValueField);
XLabel		mmLabel(159, kLabelYAdj + (kRowHeight*2), 34, 26,
				kMMLabelTag, &pinValueStepper, kMMStr,
				&UI20ptFont, nullptr,
				XFont::eBlack, kDialogBGColor, XFont::eAlignRight);
XPushButton resetBtn(0, (kRowHeight*2), 80, 0,
				kResetBtnTag, &mmLabel, kResetStr,
				&UI20ptFont,
				XFont::eWhite, kDialogBGColor);

XDialogBox	cutKeyDialog(&resetBtn,
				kCutKeyDialogTag, &adjustOriginDialog,
				kCutStr, kCancelStr, kCutKeyStr,
				&UI20ptFont,
				nullptr, kDialogBGColor);

// Utilities dialog
XLabel		motorSpeedLabel(0, kLabelYAdj, 172, 26,
				kMotorSpeedLabelTag, nullptr, kMotorSpeedStr,
				&UI20ptFont, nullptr,
				XFont::eBlack, kDialogBGColor, XFont::eAlignRight);
XNumberValueField	motorSpeedValueField(0,0,50+kSpaceBetween,
				kMotorSpeedValueFieldTag, &motorSpeedLabel,
				&UI20ptFont, 0, 128, 0, 4, false, false,
				&ValueFormatter::Int32ToString,
				0xFB00, kDialogBGColor);
XStepper	motorSpeedStepper(247, 0, 0, 0,
				kMotorSpeedStepperTag, &motorSpeedValueField);
XPushButton motorStartStopBtn(264+kSpaceBetween, 0, 80, 0,
				kMotorStartStopBtnTag, &motorSpeedStepper, kStartStr,
				&UI20ptFont,
				XFont::eWhite, kDialogBGColor);
XLabel		steppersLabel(0, kRowHeight+kLabelYAdj, 172, 26,
				kSteppersLabelTag, &motorStartStopBtn, kSteppersStr,
				&UI20ptFont, nullptr,
				XFont::eBlack, kDialogBGColor, XFont::eAlignRight);
XPushButton homeSteppersBtn(184, kRowHeight, 80, 0,
				kHomeSteppersBtnTag, &steppersLabel, kHomeStr,
				&UI20ptFont,
				XFont::eWhite, kDialogBGColor);
XPushButton steppersDisableBtn(264+kSpaceBetween, kRowHeight, 80, 0,
				kSteppersDisableBtnTag, &homeSteppersBtn, kDisableStr,
				&UI20ptFont,
				XFont::eWhite, kDialogBGColor);
XLabel		saveSettingsLabel(0, kLabelYAdj + (kRowHeight*2), 172, 26,
				kSaveSettingsLabelTag, &steppersDisableBtn, kSaveSettingsStr,
				&UI20ptFont, nullptr,
				XFont::eBlack, kDialogBGColor, XFont::eAlignRight);
XPushButton saveSettingsBtn(184, kRowHeight*2, 80, 0,
				kSaveSettingsBtnTag, &saveSettingsLabel, kSaveStr,
				&UI20ptFont,
				XFont::eWhite, kDialogBGColor);
XPushButton loadSettingsBtn(264+kSpaceBetween, kRowHeight*2, 80, 0,
				kLoadSettingsBtnTag, &saveSettingsBtn, kLoadStr,
				&UI20ptFont,
				XFont::eWhite, kDialogBGColor);
XDateValueField	dateValueField(0, kRowHeight*3, 0,
				kDateValueFieldTag, &loadSettingsBtn,
				&UI20ptFont,
				XFont::eBlack, kDialogBGColor);
XStepper	dateValueStepper(247, kRowHeight*3, 0, 0,
				kDateValueStepperTag, &dateValueField);
XPushButton setTimeBtn(264+kSpaceBetween, kRowHeight*3, 80, 0,
				kSetTimeBtnTag, &dateValueStepper, kSetStr,
				&UI20ptFont,
				XFont::eWhite, kDialogBGColor);
XDialogBox	utilitiesDialog(&setTimeBtn,
				kUtilitiesDialogTag, &cutKeyDialog,
				kCloseStr, nullptr, kUtilitiesStr,
				&UI20ptFont,
				nullptr, kDialogBGColor);

// About box
XLabel		softwareNameLabel(0, 0, 280, 26,
				kSoftwareNameLabelTag, nullptr, kSoftwareNameStr,
				&UI20ptFont, nullptr,
				XFont::eBlack, kDialogBGColor, XFont::eAlignCenter);
XLabel		versionLabel(0, kRowHeight, 280, 26,
				kVersionLabelTag, &softwareNameLabel, kVersionStr,
				&UI20ptFont, nullptr,
				XFont::eBlack, kDialogBGColor, XFont::eAlignCenter);
XLabel		copyrightLabel(0, kRowHeight*2, 280, 26,
				kCopyrightLabelTag, &versionLabel, kCopyrightStr,
				&UI20ptFont, nullptr,
				XFont::eBlack, kDialogBGColor, XFont::eAlignCenter);
XDialogBox	aboutBox(&copyrightLabel,
				kAboutBoxTag, &utilitiesDialog,
				kCloseStr, nullptr, nullptr,
				&UI20ptFont,
				nullptr, kDialogBGColor);
				
// Info view
XDateValueField	infoDateValueField(136,3,0,
				kInfoDateValueFieldTag, nullptr,
				&UI20ptFont);

XColoredView infoView(0, 0, 480, 320,
				kInfoViewTag, &aboutBox, &infoDateValueField,
				nullptr, false);

XRootView	rootView(&infoView);

#endif // KMXViews_h
