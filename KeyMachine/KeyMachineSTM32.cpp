/*
*	KeyMachineSTM32.cpp, Copyright Jonathan Mackey 2024
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
#include <Arduino.h>
#include "KeyMachineSTM32.h"
#include "SdFat.h"
#include <Wire.h>

XFont	xFont;
#if 0
// Using 1-bit fonts saves about 11.5KB over 8-bit. (MyriadPro 7.7KB + Avenir 3.8KB)
// 1-bit draws slightly faster.
#define UI20ptFont	MyriadPro_Regular_20_1b::font
#include "MyriadPro-Regular_20_1b.h"
#define UI64ptFont	Avenir_64_1b::font
#include "Avenir_64_1b.h"
#else
// 8-bit fonts (antialiased)
#define UI20ptFont	MyriadPro_Regular_20::font
#include "MyriadPro-Regular_20.h"
#define UI64ptFont	Avenir_64::font
#include "Avenir_64.h"
#endif
#include "KM_Icons.h"
#include "KMSettings.h"
#include "KMXViews.h"

static const char kKMSettingsPath[] = "KMSettings.txt";
/*
*	The SKeySpecs are hardcoded.  The KeySpec can load a SKeySpec from a text
*	file on SD.  This code has been debugged.  If SKeySpecs are loaded
*	dynamically changes would have to be made to create XMenuItems dynamically.
*	In addition, support for scrolling would have to be added to XMenu.
*/
SKeySpec	schlageKeySpec = {"Schlage", 0.83909963117, 0.015, 0.200, 0.343, 0.031, 0.1562, 0.231, 7, 0, 9, false, true, kSchlageSC1MenuItem, SKeySpec::e5PinMask};
SKeySpec	kwiksetKeySpec = {"Kwikset", 1, 0.023, 0.191, 0.335, 0.084, 0.150, 0.247, 4, 1, 7, false, true, kKwiksetKW1MenuItem, SKeySpec::e6PinMask | SKeySpec::e4PinMask};

/***************************** KeyMachineSTM32 *****************************/
KeyMachineSTM32::KeyMachineSTM32(void)
  : mDisplay(Config::kDispDCPin, Config::kDispResetPin,
						Config::kDispCSPin, Config::kBacklightPin),
	mPreferences(Config::kAT24CDeviceAddr, Config::kAT24CDeviceCapacity),
    mTouchScreen(Config::kTouchCSPin, Config::kTouchIRQPin,
			Config::kDisplayHeight, Config::kDisplayWidth,
			0, 0, 0, 0, Config::kInvertTouchX),
	mMotorIsRunning(false),
	mButtonDebouncePeriod(DEBOUNCE_DELAY), mButtonPressed(false),
	mEmergencyButtonDebouncePeriod(DEBOUNCE_DELAY), mEmergencyBtnPressed(false),
	mPOT(Config::kMCP45X1DeviceAddr),
	mStepperX(Config::kXStepPin, Config::kXDirPin),
	mStepperZ(Config::kZStepPin, Config::kZDirPin),
	mXMinEndstop(Config::kXMinEndstopPin),
	mXMaxEndstop(Config::kXMaxEndstopPin),
	mZMinEndstop(Config::kZMinEndstopPin),
	mZMaxEndstop(Config::kZMaxEndstopPin),
	mHomeXEndstop(&mController, &mStepperX, &mXMaxEndstop, -1),
	mHomeZEndstop(&mController, &mStepperZ, &mZMaxEndstop, -1),
	mFastMoveXTo{FastMoveTo(&mController, &mStepperX), FastMoveTo(&mController, &mStepperX)},
	mFastMoveZTo{FastMoveTo(&mController, &mStepperZ), FastMoveTo(&mController, &mStepperZ)},
	mCutKey(&mController, &mStepperX, &mStepperZ),
	mEndstopChanged(nullptr), mSteppersHomed(false)
{
}

/************************************ begin ***********************************/
void KeyMachineSTM32::begin(void)
{
	pinMode(Config::kEmergencyStopPin, INPUT_PULLUP);
	pinMode(Config::kCancelBtnPin, INPUT_PULLUP);
	pinMode(Config::kEnterBtnPin, INPUT_PULLUP);
	pinMode(Config::kMotorEnablePin, OUTPUT);
	digitalWrite(Config::kMotorEnablePin, LOW);

	// Define button pin interrupt routine
	// The buttons are used to setup the touchscreen.
	attachInterrupt(digitalPinToInterrupt(Config::kEmergencyStopPin),
		std::bind(&KeyMachineSTM32::EmergencyStopISR, this), FALLING);	// PA9 Ext Int 9
	attachInterrupt(digitalPinToInterrupt(Config::kCancelBtnPin),
		std::bind(&KeyMachineSTM32::ButtonPressedISR, this), FALLING);	// PA10 Ext Int 10
	attachInterrupt(digitalPinToInterrupt(Config::kEnterBtnPin),
		std::bind(&KeyMachineSTM32::ButtonPressedISR, this), FALLING);	// PA11 Ext Int 11

	/*
	*	The following pins are not used and are pulled up to prevent them from
	*	floating.
	*/
	pinMode(Config::kPDataPin, INPUT_PULLUP);		// PA12
	pinMode(Config::kYStepPin, INPUT_PULLUP);		// PA15
	pinMode(Config::kEXDirPin, INPUT_PULLUP);		// PB3
	pinMode(Config::kEXStepPin, INPUT_PULLUP);		// PB4 PWM TIM3 CH1
	pinMode(Config::kYMinEndstopPin, INPUT_PULLUP);	// PC2 Ext Int 2
	pinMode(Config::kYMaxEndstopPin, INPUT_PULLUP);	// PC3 Ext Int 3
	pinMode(Config::kYDirPin, INPUT_PULLUP);		// PC11
	pinMode(Config::kRTCOutPin, INPUT_PULLUP);		// PC13
	
	/*
	*	The status LEDs are PA0 and PA1
	*	PA0 (Red) - Error
	*	PA1 (Blue) - steppers enable state
	*/
	pinMode(Config::kLEDA0Pin, OUTPUT);	// AKA Config::kErrorPin
	pinMode(Config::kLEDA1Pin, OUTPUT);	// AKA Config::kSteppersEnabledPin
	digitalWrite(Config::kLEDA0Pin, LOW);
	
	/*
	*	Stepper driver enable pins on the 8825 are internally pulled down.
	*/
	pinMode(Config::kXEnablePin, OUTPUT);
	pinMode(Config::kYEnablePin, INPUT_PULLUP);		// Not used
	pinMode(Config::kZEnablePin, OUTPUT);
	pinMode(Config::kEXEnablePin, INPUT_PULLUP);	// Not used
	
	/*
	*	Initially disable the steppers.
	*/
	DisableSteppers();
	
	/*
	*	The fault pin on the 8825 chip is open collector and therefore can only
	*	pull the signal to ground. This pin is tied to the module's fault pin by
	*	a 1.5K resistor on the module. The fault pin on the module is also tied
	*	to the sleep pin via a 10K resistor so that the module is disabled when
	*	there's a fault.  On the main board the sleep and reset pins are tied
	*	together for the same reason.
	*/
	pinMode(Config::kXFaultPin, INPUT_PULLUP);
	pinMode(Config::kYFaultPin, INPUT_PULLUP);		// Not used
	pinMode(Config::kZFaultPin, INPUT_PULLUP);
	
	STM32UnixRTC::RTCInit();

	pinMode(Config::kSDDetectPin, INPUT_PULLUP);
	pinMode(Config::kSDSelectPin, OUTPUT);
	digitalWrite(Config::kSDSelectPin, HIGH);	// Deselect the SD card.
	
	{
		Wire.beginTransmission(Config::kAT24CDeviceAddr);
		bool	at24C64IsResp = Wire.endTransmission(true) == 0;
	
		/*
		*	Load the preferences...
		*	If the EEPROM is responding...
		*/
		if (at24C64IsResp)
		{
			Config::SKMPreferences	prefs;
			bool	prefsRead = mPreferences.Read(0, sizeof(Config::SKMPreferences), &prefs.unused[0]);
			if (prefsRead)
			{
				//Serial.printf("prefsRead\n");
				UnixTime::SetFormat24Hour(prefs.clockFormat == 0);
		
				/*
				*	Initialize the touchscreen alignment values if previously saved
				*/
				if (prefs.tsMinMax[0] != 0xFFFF)
				{
					mTouchScreen.SetMinMax(prefs.tsMinMax);
					//Serial.printf("prefs.tsMinMax: %hd, %hd, %hd, %hd\n",
					//	prefs.tsMinMax[0], prefs.tsMinMax[1], prefs.tsMinMax[2], prefs.tsMinMax[3]);
				}
			}
		} else
		{
			Serial.printf("AT24C64 is not responding.\n");
		}
	}
	//Serial.printf("MCU Frequency = %d\n", F_CPU);
	{
		/*
		*	Allocate hardware timers.
		*	The allocation must happen in the order below.  The stepper pulse
		*	timers are tied to a specific pin, whereas the controller step timer
		*	can be any hardware timer. For that reason the controller gets
		*	allocated after the pulse timers.
		*/
		mStepperX.setupPulseTimer();	// Allocates timer TIM4 CH3 PWM
		mStepperZ.setupPulseTimer();	// Allocates timer TIM8 CH4 PWM
		mController.getTimerField().begin();
		/*
		*	Debugging the above timer assignments...
		*
		*	Expect only 3 bits set, one for each stepper pulse timer and the
		*	common step timer. The stepTimer timer owned by the control won't be
		*	set till the first move.
		*/
		//Serial.printf("sReservedTimers = 0x%X\n", STMTimers::GetReservedTimers());

		/*
		*	Binding addresses doesn't work in the corresponding constructors so
		*	it's performed here.  It makes sense to do it here anyway because
		*	mController is now initialized at this point.
		*/
		{
		    using namespace std::placeholders;

			mXMinEndstop.begin(std::bind(&KeyMachineSTM32::EndstopChangedISR, this, _1, _2));
			mXMaxEndstop.begin(std::bind(&KeyMachineSTM32::EndstopChangedISR, this, _1, _2));
			mZMinEndstop.begin(std::bind(&KeyMachineSTM32::EndstopChangedISR, this, _1, _2));
			mZMaxEndstop.begin(std::bind(&KeyMachineSTM32::EndstopChangedISR, this, _1, _2));
			
			mHomeXEndstop.SetCallback(std::bind(&KeyMachineSTM32::EndstopsHomed, this, _1, _2));
			
			mStartMotor.SetCallback(std::bind(&KeyMachineSTM32::StartKMMotor, this, _1, _2));
			mStartMotor.SetWaitPeriod(0, 5000);	// Wait after starting
			mStopMotor.SetCallback(std::bind(&KeyMachineSTM32::StopKMMotor, this, _1, _2));
		}
	}

	{
		Wire.beginTransmission(Config::kMCP45X1DeviceAddr);
		mMCP45X1Exists = Wire.endTransmission(true) == 0;
		if (mMCP45X1Exists)
		{
			mPOT.Write(128);	// Set the motor speed.  128 = max speed.
		} else
		{
			motorSpeedValueField.Enable(false, false);
			motorSpeedStepper.Enable(false, false);
			Serial.printf("MCP45X1 is not responding.\n");
		}
	}
	
	/*
	*	Setup the display...
	*/
	mTouchScreen.begin(Config::kDisplayRotation);
	mDisplay.begin(Config::kDisplayRotation);	// Init TFT
	
	rootView.SetSize(Config::kDisplayHeight, Config::kDisplayWidth);
	rootView.SetDisplay(&mDisplay);
	rootView.SetModalView(&mainMenuBtn);
	rootView.SetViewChangedDelegate(this);
	warningDialog.SetViewChangedDelegate(this);
	warningDialog.SetMinDialogSize();
	xFont.SetDisplay(&mDisplay, &UI20ptFont);	// To initialize mDisplay of xFont

	/*
	*	Move the Reset button within the Cut Key dialog...
	*
	*	In order to have the reset button on the last line with the Cancel and
	*	Cut buttons, Autosize the dialog to position the views, then move the
	*	reset button.  This can't be done using constructor params because
	*	placing the reset button on the last line would cause the dialog height
	*	to increase within AutoSize.  Note that AutoSize is only applied once.
	*/
	cutKeyDialog.AutoSize();
	resetBtn.SetOrigin(resetBtn.X(), cutKeyDialog.GetOKButton()->Y());
	
	ShowInfoView();
}

/*************************** NoModalDialogDisplayed ***************************/
bool KeyMachineSTM32::NoModalDialogDisplayed(void) const
{
	return(rootView.ModalView() == &mainMenuBtn);
}

/*********************************** Update ***********************************/
/*
*	Called from loop()
*/
bool KeyMachineSTM32::Update(void)
{
	if (mTouchScreen.PenStateChanged())
	{
		if (mTouchScreen.PenIsDown())
		{
			if (!mDisplaySleeping)
			{
				//Serial.printf("D\n");
				UnixTime::ResetSleepTime();
				uint16_t	z;
				mTouchScreen.Read(mX, mY, z);
				mHitView = rootView.HitTest(mX, mY);
				if (mHitView)
				{
					mHitView->MouseDown(mX,mY);
				}
			} else
			{
				WakeUp();
			}
		} else	// Else, pen is up
		{
			//Serial.printf("U\n");
			if (mHitView)
			{
				mHitView->MouseUp(mX, mY);
				mHitView = nullptr;
			}
		}
	}

	CheckButtons();	// Buttons are used to setup the touchscreen.
	UpdateEndstops();
	GiveTimeToActions();
	
	/*
	*	If the display isn't sleeping...
	*/
	if (!mDisplaySleeping)
	{
		bool	noModalDialogDisplayed = NoModalDialogDisplayed();
		/*
		*	If there are no modal dialogs visible THEN
		*	update the info view.
		*/
		if (noModalDialogDisplayed)
		{
			if (!mainMenu.IsVisible())
			{
				UpdateInfoView();
			}
		} else if (UnixTime::TimeChanged())
		{
			UnixTime::ResetTimeChanged();

			if (setupKeyHolderOriginDialog.IsVisible())
			{	// Update value (updates only if changed)
				currentXValueField.SetValue(TO_STEPS(mStepperX.getPosition()), true);
				currentZValueField.SetValue(TO_STEPS(mStepperZ.getPosition()), true);
			}
		}
		
		kmStatusIcon.Update();
		if (UnixTime::TimeToSleep())
		{
			if (noModalDialogDisplayed &&
				!mMotorIsRunning)
			{
				GoToSleep();
			} else
			{
				UnixTime::ResetSleepTime();
			}
		}
	} else if (mMotorIsRunning)
	{
		WakeUp();
	}

#if 1
	/*
	*	Serial commands
	*/
	if (Serial.available())
	{
		switch (Serial.read())
		{
			case '>':
				Serial.printf(">\n");
				// Set the time.  A hexadecimal ASCII UNIX time follows
						// Use >65920071 for all-fields-change test (15s delay)
				UnixTime::SetUnixTimeFromSerial();
				STM32UnixRTC::SyncRTCToTime();
				break;
		}
	}
#endif	

	return(false);
}

/*********************************** WakeUp ***********************************/
/*
*	Wakup the display from sleep and/or keep it awake if not sleeping.
*/
void KeyMachineSTM32::WakeUp(void)
{
	if (mDisplaySleeping)
	{
		mDisplaySleeping = false;
		mDisplay.WakeUp();
		rootView.Draw(0, 0, 999, 999);
	}
	UnixTime::ResetSleepTime();
}

/********************************* GoToSleep **********************************/
/*
*	Puts the display to sleep.
*/
void KeyMachineSTM32::GoToSleep(void)
{
	if (!mDisplaySleeping)
	{
		mDisplay.Fill();
		mDisplay.Sleep();
		mDisplaySleeping = true;
	}
}

/****************************** GiveTimeToActions *****************************/
void KeyMachineSTM32::GiveTimeToActions(void)
{
	KMActionQueue::EActionQueueState	queueState = mActionQueue.ContinueAction();
	if (mLastActionQueueState != queueState)
	{
		mLastActionQueueState = queueState;
	#if 0
		switch(queueState)
		{
			case KMActionQueue::eQueueEmpty:
			{
				Serial.printf("Action Queue Empty\n");
				break;
			}
			case KMActionQueue::eActionPending:
			{
				Serial.printf("Action Pending\n");
				break;
			}
			case KMActionQueue::eActionExecuting:
			{
				Serial.printf("Action \"%s\" Executing\n",
								mActionQueue.Current()->Name());
				break;
			}
			case KMActionQueue::eActionFailed:
			{
				Serial.printf("Action \"%s\" Failed, exit = %d\n",
								mActionQueue.Current()->Name(),
								mActionQueue.Current()->ExitState());
				mActionQueue.Clear();
				break;
			}
		}
	#else
		if (queueState == KMActionQueue::eActionFailed)
		{
			Serial.printf("Action \"%s\" Failed, exit = %d\n",
							mActionQueue.Current()->Name(),
							mActionQueue.Current()->ExitState());
			mActionQueue.Clear();
		}
	#endif
	}
}

/******************************* UpdateEndstops *******************************/
void KeyMachineSTM32::UpdateEndstops(void)
{
	if (mEndstopChanged)
	{
	#if 0
		/*
		*	Whenever an endstop is tripped, stop the respective stepper.
		*/
		switch (mEndstopChanged->GetPortPinMask())
		{
			case Config::kXMinEndstopMask:
				Serial.printf("XMin endstop %s\n", mEndstopChanged->AtEndstop() ? "AT":"OFF");
				break;
			case Config::kXMaxEndstopMask:
				Serial.printf("XMax endstop %s\n", mEndstopChanged->AtEndstop() ? "AT":"OFF");
				break;
			case Config::kZMinEndstopMask:
				Serial.printf("ZMin endstop %s\n", mEndstopChanged->AtEndstop() ? "AT":"OFF");
				break;
			case Config::kZMaxEndstopMask:
				Serial.printf("ZMax endstop %s\n", mEndstopChanged->AtEndstop() ? "AT":"OFF");
				break;
		}
	#endif
		mEndstopChanged = nullptr;
	}
}

/******************************** CheckButtons ********************************/
/*
*	CheckButtons is used to setup the display orientation and the touch screen
*	alignment values. 
*/
void KeyMachineSTM32::CheckButtons(void)
{
	if (mButtonPressed)
	{
		uint32_t	btnPinsState = (~GPIOA->IDR) & Config::kPINABtnMask;
		if (btnPinsState)
		{
			/*
			*	If the state hasn't changed since last check
			*/
			if (mButtonPinState == btnPinsState)
			{
				/*
				*	Wakeup the display when any key is pressed.
				*/
				WakeUp();
				/*
				*	If a debounce period has passed
				*/
				if (mButtonDebouncePeriod.Passed())
				{
					UnixTime::ResetSleepTime();
					mButtonPressed = false;
					mButtonPinState = 0xFF;
					switch (btnPinsState)
					{
						case Config::kCancelBtnMask:	// Cancel button pressed
							if (touchScreenAlignment.IsVisible())
							{
								// Stop and cancel/quit (restore old alignment)
								//Serial.printf("Cancel Align\n");
								infoView.SetVisible(true);
								touchScreenAlignment.Stop(true);
							}
							break;
						case Config::kEnterBtnMask:	// Enter button pressed
							if (touchScreenAlignment.IsVisible())
							{
								if (touchScreenAlignment.OKToSave())
								{
									// Stop and save
									infoView.SetVisible(true);
									touchScreenAlignment.Stop(false);
									uint16_t	minMax[4];
									mTouchScreen.GetMinMax(minMax);
									mPreferences.Write(offsetof(Config::SKMPreferences, tsMinMax), sizeof(minMax), (const uint8_t*)minMax);
								}
							} else if (NoModalDialogDisplayed())
							{
								infoView.SetVisible(false);
								touchScreenAlignment.Start(&mTouchScreen);
							}
							break;
						default:
							mButtonDebouncePeriod.Start();
							break;
					}
				}
			} else
			{
				mButtonPinState = btnPinsState;
				mButtonDebouncePeriod.Start();
			}
		}
	} else if (mEmergencyBtnPressed &&
		mEmergencyButtonDebouncePeriod.Passed())
	{
		mEmergencyBtnPressed = false;
		mActionQueue.Clear();
		Serial.printf("EmergencyStopISR\n");
	}

}

/******************************* UpdateInfoView *******************************/
/*
*	Called from Update() when no modal dialogs are displayed.
*/
void KeyMachineSTM32::UpdateInfoView(void)
{
	if (infoView.IsVisible())
	{
		if (UnixTime::TimeChanged())
		{
			UnixTime::ResetTimeChanged();
			infoDateValueField.SetValue(UnixTime::Time());
		}
	}
}

/******************************** StopSteppers ********************************/
void KeyMachineSTM32::StopSteppers(void)
{
	mController.emergencyStop();
}

/****************************** ButtonPressedISR ******************************/
/*
*	Called via an interrupt.  The button press is handled in CheckButtons()
*	after debouncing.
*/
void KeyMachineSTM32::ButtonPressedISR(void)
{
	mButtonPressed = true;
}

/****************************** EndstopChangedISR *****************************/
void KeyMachineSTM32::EndstopChangedISR(
	Endstop*	inEndstop,
	bool		inAtEndstop)
{
	mEndstopChanged = inEndstop;
	if (inAtEndstop)
	{
		StopSteppers();
	}
}

/****************************** EmergencyStopISR ******************************/
void KeyMachineSTM32::EmergencyStopISR(void)
{
	/*
	*	mEmergencyBtnPressed and mEmergencyButtonDebouncePeriod are used to
	*	avoid button bounce.
	*/
	if (mEmergencyBtnPressed == false)
	{
		mEmergencyBtnPressed = true;	// Reset in CheckButtons() after the
		mEmergencyButtonDebouncePeriod.Start();	// debounce period expires.
		mController.emergencyStop();
		DisableSteppers();
		StopKMMotor();
	}
}
	
/******************************* ValuesAreValid *******************************/
/*
*	ValuesAreValid is a member of the XValidatorDelegate mixin class.
*	It's called when a dialog's OK button is pressed.
*	True is returned if the values are valid and the dialog can be dismissed,
*	otherwise the dialog either just stays visible (no action) or the dialog
*	displays an error explaining why the dialog can't be dismissed.
*/
bool KeyMachineSTM32::ValuesAreValid(
	XDialogBox*	inDialog)
{
	bool	valuesAreValid = false;
	if (inDialog)
	{
		switch (inDialog->Tag())
		{
			case kCutKeyDialogTag:
			{
				SKeySpec::EErrorCode	errCode = pinsValueField.ValuesAreValid();
				valuesAreValid = errCode == SKeySpec::eNoErr;
				if (!valuesAreValid)
				{
					const char*	errStr = nullptr;
					switch((uint16_t)errCode)
					{
						case SKeySpec::eMACSErr:
							errStr = kMACSMessageStr;
							break;
						case SKeySpec::eDepthMaxErr:
							errStr = kDepthErrMessageStr;
							break;
						case SKeySpec::ePinIndexErr:
							errStr = kPinIndexMessageStr;
							break;
					}
					if (errStr)
					{
						warningDialog.DoMessage(errStr);
					}
				}
				break;
			}
			case kSetupKeyHolderDialogTag:
			{
				if (mController.isRunning() == false)
				{
					warningDialog.DoMessage(kSteppersBusyStr);
				}
				break;
			}
			
		}
	}
	return(valuesAreValid);
}

/****************************** HandleViewChange ******************************/
void KeyMachineSTM32::HandleViewChange(
	XView*		inView,
	uint16_t	inAction)
{
	if (inView)
	{
	#if 0
		Serial.printf("Change: Tag = %hd, Action = %hd\n", inView->Tag(), inAction);
	#endif
		switch (inView->Tag())
		{
			case kMainMenuTag:
				switch (inAction)
				{
					case kInfoMenuItem:
						ShowInfoView();
						break;
					case kCutKeyMenuItem:
						ShowCutKeyDialog();
						break;
					case kSetupZeroMenuItem:
						ShowSetupZeroDialog();
						break;
					case kAdjustOriginMenuItem:
						ShowAdjustOriginDialog();
						break;
					case kUtilitiesMenuItem:
						ShowUtilitiesDialog();
						break;
					case kAboutMenuItem:
						aboutBox.Show();
						break;
				}
				break;
			case kSetupKeyHolderDialogTag+XDialogBox::eOKTagOffset:
			{
				if (inAction == 1)
				{
					SaveSetupKeyHolderDialogChanges();
				}
				break;
			}
			case kSetupKeyHolderDialogTag+XDialogBox::eCancelTagOffset:
			{
				if (inAction == 1)
				{
					mActionQueue.Clear();
					mController.emergencyStop();
					mSteppersHomed = false;
					HomeAndMoveCutterHeadTo(0, 0);
				}
				break;
			}
			case kAdjustOriginDialogTag+XDialogBox::eOKTagOffset:
			{
				if (inAction == 1)
				{
					SaveAdjustOriginDialogChanges();
				}
				break;
			}
			case kUtilitiesDialogTag+XDialogBox::eOKTagOffset:
			{
				/*
				*	When exiting the Utilities dialog, don't leave
				*	the motor running...
				*/
				if (inAction == 1 &&
					mMotorIsRunning)
				{
					StopKMMotor();
				}
				break;
			}
			case kIncrementMenuTag:
				zeroXOffsetValueField.SetIncrement(inAction);
				zeroZOffsetValueField.SetIncrement(inAction);
				break;
			case kUpBtnTag:
			{
				if (inAction == 1)
				{
					int32_t increment = zeroXOffsetValueField.GetIncrement();
					if (TO_STEPS(mStepperZ.getPosition()) > increment)
					{
						mFastMoveZTo[0].SetSteps(-increment, true);
						mActionQueue.AppendAction(&mFastMoveZTo[0]);
					}
				}
				break;
			}
			case kDownBtnTag:
			{
				if (inAction == 1)
				{
					int32_t increment = zeroXOffsetValueField.GetIncrement();
					if (Config::kKeyHolderRoughZMaxSteps - TO_STEPS(mStepperZ.getPosition()) > increment)
					{
						mFastMoveZTo[0].SetSteps(increment, true);
						mActionQueue.AppendAction(&mFastMoveZTo[0]);
					}
				}
				break;
			}
			case kLeftBtnTag:
			{
				if (inAction == 1)
				{
					int32_t increment = zeroXOffsetValueField.GetIncrement();
					mFastMoveXTo[0].SetSteps(increment, true);
					mActionQueue.AppendAction(&mFastMoveXTo[0]);
				}
				break;
			}
			case kRightBtnTag:
			{
				if (inAction == 1)
				{
					int32_t increment = zeroXOffsetValueField.GetIncrement();
					if (TO_STEPS(mStepperX.getPosition())-Config::kKeyHolderRoughXMaxSteps > increment)
					{
						mFastMoveXTo[0].SetSteps(-increment, true);
						mActionQueue.AppendAction(&mFastMoveXTo[0]);
					}
				}
				break;
			}
			case kCutKeyDialogTag+XDialogBox::eOKTagOffset:
				if (inAction == 1)
				{
					DoCutKey();
				}
				break;
			case kPinsValueFieldTag:
			case kPinsValueStepperTag:
				if (inAction == 0)
				{
					pinDepthValueField.SetValue(pinsValueField.GetActivePinDepth());
				}
				break;
			case kPinValueStepperTag:
				pinsValueField.SetActivePinDepth(pinDepthValueField.Value());
				break;
			case kKeywayMenuTag:
				LoadKeySpecByTag(inAction);
				break;
			case kPinCountMenuTag:
				if (pinsValueField.GetPinCount() != inAction)
				{
					pinsValueField.SetPinCount(inAction);
				}
				break;
			case kResetBtnTag:
				if (inAction == 1)
				{
					uint16_t	keySpecToResetTag = pinsValueField.GetKeySpec().tag;
					pinsValueField.GetKeySpec().tag = 99;	// To force a reset
					LoadKeySpecByTag(keySpecToResetTag);
				}
				break;
			case kAlertDialogTag+XDialogBox::eOKTagOffset:
				switch(alertDialog.MessageTag())
				{
					case kLoadSettingsBtnTag:
						LoadKMSettingsFromSD();
						break;
				}
				break;
			case kMotorStartStopBtnTag:
			{
				if (inAction == 1)
				{
					if (mMotorIsRunning)
					{
						StopKMMotor();
						motorStartStopBtn.SetString(kStartStr);
					} else
					{
						StartKMMotor();
						motorStartStopBtn.SetString(kStopStr);
					}
				}
				break;
			}
			case kMotorSpeedStepperTag:
				if (inAction == 0 &&
					mMCP45X1Exists)
				{
					//bool success = mPOT.Increment();
					bool success = mPOT.Write(motorSpeedValueField.Value());
					//Serial.printf("%c, mPOT read= %hd\n", success ? 'Y':'N', mPOT.Read());
				}
				break;
			case kHomeSteppersBtnTag:
			{
				if (inAction == 1)
				{
					HomeAndMoveCutterHeadTo(0, 0);
				}
				break;
			}
			case kSteppersDisableBtnTag:
			{
				if (inAction == 1)
				{
					mActionQueue.Clear();
					mController.emergencyStop();
					DisableSteppers();
				}
				break;
			}
			case kSaveSettingsBtnTag:
				if (inAction == 0)
				{
					SaveKMSettingsToSD();
				}
				break;
			case kLoadSettingsBtnTag:
				if (inAction == 0)
				{
					if (digitalRead(Config::kSDDetectPin))
					{
						warningDialog.DoMessage(kNoSDCardFoundStr);
					} else
					{
						alertDialog.DoMessage(kLoadKMSettingsStr, kLoadSettingsBtnTag);
					}
				}
				break;			
			case kSetTimeBtnTag:
			{
				if (inAction == 1)
				{
					UnixTime::SetTime(dateValueField.Value());
					STM32UnixRTC::SyncRTCToTime();
				}
				break;
			}
		}
	}
}

/******************************** StartKMMotor ********************************/
void KeyMachineSTM32::StartKMMotor(
	KMAction*	inAction,
	uint32_t	inExitState)
{
	mMotorIsRunning = true;
	digitalWrite(Config::kMotorEnablePin, HIGH);
	kmStatusIcon.SetAnimationPeriod(750);
}

/******************************** StopKMMotor *********************************/
void KeyMachineSTM32::StopKMMotor(
	KMAction*	inAction,
	uint32_t	inExitState)
{
	mMotorIsRunning = false;
	digitalWrite(Config::kMotorEnablePin, LOW);
	kmStatusIcon.SetAnimationPeriod(0);
}

/******************************** ShowInfoView ********************************/
void KeyMachineSTM32::ShowInfoView(void)
{
	if (!infoView.IsVisible())
	{
		infoView.SetVisible(true);
		rootView.Draw(0, 0, 480, 320);
	}
}

/**************************** ShowUtilitiesDialog *****************************/
void KeyMachineSTM32::ShowUtilitiesDialog(void)
{
	if (mActionQueue.IsEmpty())
	{
		if (mMCP45X1Exists)
		{
			motorSpeedValueField.SetValue(mPOT.Read(), false);
		}
		motorStartStopBtn.SetString(mMotorIsRunning ? kStopStr : kStartStr);
		dateValueField.SetValue(UnixTime::Time(), false);
		utilitiesDialog.Show();
	} else
	{
		warningDialog.DoMessage(kSteppersBusyStr);
	}
}

/*************************** ShowAdjustOriginDialog ***************************/
/*
*	This makes fine adjustments to the origin.  The origin is the distance from
*	the max endstops to the key shoulder and the bottom of the key holder.
*
*	Increasing X moves all cuts to the right.
*	Increasing Z moves all cuts down.
*/
void KeyMachineSTM32::ShowAdjustOriginDialog(void)
{
	/*
	*	Only show the dialog if the steppers are idle.
	*/
	if (mActionQueue.IsEmpty())
	{
		uint32_t	keyHolderOriginX = 0;
		uint32_t	keyHolderOriginZ = 0;
		/*
		*	If the key holder origin has been defined THEN
		*	get and load the last saved values.
		*/
		if (GetKeyHolderOrigin(keyHolderOriginX, keyHolderOriginZ))
		{
			deltaXValueField.SetValue(0, false);
			deltaZValueField.SetValue(0, false);
			adjustOriginDialog.Show();
		} else
		{
			warningDialog.DoMessage(kKeyHolderOriginUndefinedStr);
		}
	} else
	{
		warningDialog.DoMessage(kSteppersBusyStr);
	}
}

/************************ SaveAdjustOriginDialogChanges ***********************/
void KeyMachineSTM32::SaveAdjustOriginDialogChanges(void)
{
	Config::SKeyHolderDialogPrefs	prefs;
	bool	prefsRead = mPreferences.Read(Config::kKeyHolderDialogAddr,
							sizeof(Config::SKeyHolderDialogPrefs), (uint8_t*)&prefs);
	if (prefsRead)
	{
		prefs.keyHolderOriginX += deltaXValueField.Value();
		prefs.keyHolderOriginZ += deltaZValueField.Value();
		
		mPreferences.Write(Config::kKeyHolderDialogAddr, sizeof(Config::SKeyHolderDialogPrefs), (uint8_t*)&prefs);
	}
}

/**************************** ShowSetupZeroDialog *****************************/
/*
*	When showing the Setup zero dialog, the rough values, kKeyHolderRoughXSteps
*	and kKeyHolderRoughYSteps are where the cutter will initially be placed. 
*	From there you can't move more than 1mm right, or 5.4mm down.  In practice
*	the cutter will hit the key used for alignment before this occurs.  There's
*	no way to stop you from coming in contact with the sample key but there are
*	some limits with the holder itself.
*
*	The zero offset dialog values entered:
*				Z is the width of the sample key blade being used.
*				X is the deviation of the sample key bottom shoulder with the
*					top shoulder.  This value is almost always set to zero.
*
*	When cutting a key, the depths (Z) are relative to the bottom of the key
*	holder slot. So any Z movements are subtracted from the saved keyHolderOriginZ
*	pref.  X movements are relative to the key shoulder, so any X moves are
*	subtracted from the keyHolderOriginX pref.  This gives you the absolute X and
*	Z coordinates for the cut.
*/
void KeyMachineSTM32::ShowSetupZeroDialog(void)
{
	/*
	*	Only show the dialog if the steppers are idle.
	*/
	if (HomeAndMoveCutterHeadTo(Config::kKeyHolderRoughXSteps,
						 Config::kKeyHolderRoughZSteps))
	{
		currentXValueField.SetValue(TO_STEPS(mStepperX.getPosition()), false);
		currentZValueField.SetValue(TO_STEPS(mStepperZ.getPosition()), false);

		/*
		*	Load the dialog prefs from EEPROM
		*/
		{
			Config::SKeyHolderDialogPrefs	prefs;
			bool	prefsRead = mPreferences.Read(Config::kKeyHolderDialogAddr,
									sizeof(Config::SKeyHolderDialogPrefs), (uint8_t*)&prefs);
			if (prefsRead)
			{
				/*
				*	Steps from home position to key holder zero.  Key holder
				*	zero is the bottom left of the slot in the key holder that
				*	holds a key.  For most keys the left is the key's shoulder.
				*/
				
				if (prefs.keyHolderOriginX == 0xFFFFFFFF)
				{
					prefs = {0};
				}
				
				/*
				*	The minimum Z offset is the height of the key holder. 457
				*/
				if (prefs.keyHolderZeroOffsetZ < Config::kKeyHolderDepth)
				{
					prefs.keyHolderZeroOffsetZ = Config::kKeyHolderDepth;
				}
				
				/*
				*	If the increment is invalid THEN
				*	use the default.
				*/
				if (prefs.incMenuItemTag > k10mmMenuItem ||
					prefs.incMenuItemTag < kPt01mmMenuItem)
				{
					prefs.incMenuItemTag = kPt01mmMenuItem;	// Default
				}
				zeroXOffsetValueField.SetValue(prefs.keyHolderZeroOffsetX, false);
				zeroZOffsetValueField.SetValue(prefs.keyHolderZeroOffsetZ, false);
				zeroXOffsetValueField.SetIncrement(prefs.incMenuItemTag);
				zeroZOffsetValueField.SetIncrement(prefs.incMenuItemTag);
				incrementPopUp.SelectMenuItem(prefs.incMenuItemTag);
				setupKeyHolderOriginDialog.Show();
			} else
			{
				warningDialog.DoMessage(kUnableToReadPrefsStr);
			}
		}
	} else
	{
		warningDialog.DoMessage(kSteppersBusyStr);
	}
}

/************************** HomeAndMoveCutterHeadTo ***************************/
/*
*	Moves the cutter head to inX, inZ, homing first if needed.
*	Returns true if the move could be queued.
*
*	This routine is used by the setup zero and cut key dialogs.
*/
bool KeyMachineSTM32::HomeAndMoveCutterHeadTo(
	int32_t	inX,
	int32_t	inZ)
{
	bool canDoMove = mController.isRunning() == false;
	if (canDoMove)
	{
		EnableSteppers();
		/*
		*	If the steppers have already been homed THEN
		*	move to the approximate zero key holder position relative to
		*	current.
		*/
		if (mSteppersHomed)
		{
			/*
			*	If the Z position isn't home THEN
			*	move it to the home position before moving the key holder.
			*/
			if (!mZMaxEndstop.AtEndstop())
			{
				mActionQueue.AppendAction(&mHomeZEndstop);
			}
			int32_t	moveBy = inX-TO_STEPS(mStepperX.getPosition());
			/*
			*	If the key holder needs to move...
			*/
			if (moveBy)
			{
				mFastMoveXTo[0].SetSteps(moveBy, true);
				mActionQueue.AppendAction(&mFastMoveXTo[0]);
			}
		/*
		*	Else, home the steppers then move to the approximate zero key
		*	holder position.
		*/
		} else
		{
			mActionQueue.AppendAction(&mHomeZEndstop);
			mActionQueue.AppendAction(&mHomeXEndstop);
			mFastMoveXTo[0].SetSteps(inX, true);
			mActionQueue.AppendAction(&mFastMoveXTo[0]);
		}
		mFastMoveZTo[0].SetSteps(inZ, true);
		mActionQueue.AppendAction(&mFastMoveZTo[0]);
	}
	return(canDoMove);
}

/********************** SaveSetupKeyHolderDialogChanges ***********************/
void KeyMachineSTM32::SaveSetupKeyHolderDialogChanges(void)
{
	Config::SKeyHolderDialogPrefs	prefs;
	prefs.incMenuItemTag = zeroXOffsetValueField.GetIncrement();
	prefs.keyHolderZeroOffsetX = zeroXOffsetValueField.Value();
	prefs.keyHolderZeroOffsetZ = zeroZOffsetValueField.Value();
	prefs.keyHolderOriginX = TO_STEPS(mStepperX.getPosition()) - prefs.keyHolderZeroOffsetX;
	prefs.keyHolderOriginZ = TO_STEPS(mStepperZ.getPosition()) + prefs.keyHolderZeroOffsetZ;
	
	mPreferences.Write(Config::kKeyHolderDialogAddr, sizeof(Config::SKeyHolderDialogPrefs), (uint8_t*)&prefs);
	
	mFastMoveZTo[0].SetSteps(200, true);
	mActionQueue.AppendAction(&mFastMoveZTo[0]);
}

/***************************** GetKeyHolderOrigin *****************************/
bool KeyMachineSTM32::GetKeyHolderOrigin(
	uint32_t&	outOriginX,
	uint32_t&	outOriginY)
{
	bool originIsValid = false;
	Config::SKeyHolderDialogPrefs	prefs;
	bool	prefsRead = mPreferences.Read(Config::kKeyHolderDialogAddr,
							sizeof(Config::SKeyHolderDialogPrefs), (uint8_t*)&prefs);
	if (prefsRead)
	{
		outOriginX = prefs.keyHolderOriginX;
		outOriginY = prefs.keyHolderOriginZ;
		originIsValid = (outOriginX > Config::kKeyHolderRoughXMaxSteps &&
			outOriginX < Config::kKeyHolderRoughXSteps &&
			outOriginY > Config::kKeyHolderRoughZMaxSteps &&
			outOriginY < (Config::kKeyHolderRoughZMaxSteps + Config::kKeyHolderDepth));
	}
	return(originIsValid);
}

/****************************** ShowCutKeyDialog ******************************/
void KeyMachineSTM32::ShowCutKeyDialog(void)
{
	/*
	*	Only show the dialog if the steppers are idle.
	*/
	if (!mController.isRunning())
	{
		uint32_t	keyHolderOriginX = 0;
		uint32_t	keyHolderOriginZ = 0;
		/*
		*	If the key holder origin has been defined THEN
		*	get and load the last saved values.
		*/
		if (GetKeyHolderOrigin(keyHolderOriginX, keyHolderOriginZ))
		{
			/*
			*	Load the dialog prefs from EEPROM
			*/
			Config::SCutKeyDialogPrefs	prefs;
			bool	prefsRead = mPreferences.Read(Config::kCutKeyDialogAddr,
									sizeof(Config::SCutKeyDialogPrefs), (uint8_t*)&prefs);
			if (prefsRead)
			{
				
				/*
				*	If the prefs are invalid OR
				*	the keyspec tag is invalid THEN
				*	load the default
				*/
				if (prefs.keywayMenuItemTag == 0xFFFF ||
					FindKeySpecByTag(prefs.keywayMenuItemTag) == nullptr)
				{
					prefs = {0};
					prefs.keywayMenuItemTag = kSchlageSC1MenuItem;
					prefs.pinCountMenuItemTag = k5PinMenuItem;
				}
				keywayPopUp.SelectMenuItem(prefs.keywayMenuItemTag);
				pinCountPopUp.SelectMenuItem(prefs.pinCountMenuItemTag);
				pinsValueField.SetPinCount(prefs.pinCountMenuItemTag, false);
				LoadKeySpecByTag(prefs.keywayMenuItemTag, false);
				pinsValueField.SetPinDepthsDec22mm(prefs.pinDepths);
				cutKeyDialog.Show();
			} else
			{
				warningDialog.DoMessage(kUnableToReadPrefsStr);
			}
		} else
		{
			warningDialog.DoMessage(kKeyHolderOriginUndefinedStr);
		}
	} else
	{
		warningDialog.DoMessage(kSteppersBusyStr);
	}
}

/****************************** LoadKeySpecByTag ******************************/
/*
*	Loads the SKeySpec into the Cut Key dialog.
*	Updates the state of the pinCountMenu items based on the pin counts
*	supported in the new key spec.
*/
void KeyMachineSTM32::LoadKeySpecByTag(
	uint16_t	inTag,
	bool		inUpdate)
{
	//Serial.printf("inTag = %hd, current tag = %hd\n", inTag, pinsValueField.GetKeySpec().tag);
	/*
	*	If the load isn't the result of a user action OR
	*	the keySpec has changed THEN
	*	load the keyspec.
	*/
	if (inUpdate == false ||
		pinsValueField.GetKeySpec().tag != inTag)
	{
		const SKeySpec*	keySpec = FindKeySpecByTag(inTag);
		if (keySpec)
		{
			bool	needsNewSelection = pinCountMenu.GetSelectedItem() == nullptr ||
						keySpec->PinCountSupported(pinCountMenu.GetSelectedItem()->Tag()) == false;
			if (needsNewSelection)
			{
				pinCountMenu.ClearSelectedItem();
			}
			/*
			*	Enable/Disable the items in the pinCountMenu based on the
			*	SKeySpec::pinsSupported mask.
			*/
			XMenuItem*	menuItem = pinCountMenu.GetMenuItems();
			for (; menuItem; menuItem = menuItem->NextItem())
			{
				/*
				*	If this pin count item should be disabled...
				*/
				if (keySpec->PinCountSupported(menuItem->Tag()) == false)
				{
					menuItem->SetState(XMenuItem::eDisabled);
				} else
				{
					if (needsNewSelection)
					{
						needsNewSelection = false;
						pinCountPopUp.SelectMenuItem(menuItem->Tag());
						pinsValueField.SetPinCount(menuItem->Tag(), false);
						if (inUpdate)
						{
							pinCountPopUp.DrawSelf();
						}
					} else if (menuItem->GetState() == XMenuItem::eDisabled)
					{
						menuItem->SetState(XMenuItem::eOff);
					}
				}
			}
			pinsValueField.SetKeySpec(keySpec, inUpdate);
		}
	}
}

/****************************** FindKeySpecByTag ******************************/
/*
*	Currently the SKeySpecs are hard coded.  This bottleneck could be modified
*	when SKeySpecs are loaded dynamically from either SD or EEPROM.
*/
const SKeySpec* KeyMachineSTM32::FindKeySpecByTag(
	uint16_t	inTag) const
{
	SKeySpec*	keySpec = nullptr;
	switch (inTag)
	{
		case kKwiksetKW1MenuItem:
			keySpec = &kwiksetKeySpec;
			break;
		case kSchlageSC1MenuItem:
			keySpec = &schlageKeySpec;
			break;
	}
	return(keySpec);
}

/********************************** DoCutKey **********************************/
void KeyMachineSTM32::DoCutKey(void)
{
	Config::SCutKeyDialogPrefs	prefs;
	uint32_t					errorPin;
	SKeySpec::EErrorCode errorCode = pinsValueField.GetPinDepthsDec22mm(prefs.pinDepths, &errorPin);
	/*
	*	At this point the pin depth values should have been verified within
	*	ValuesAreValid(). So errorCode should always be SKeySpec::eNoErr.
	*/
	if (errorCode == SKeySpec::eNoErr)
	{
		prefs.keywayMenuItemTag = keywayMenu.GetSelectedItem()->Tag();
		prefs.pinCountMenuItemTag = pinCountMenu.GetSelectedItem()->Tag();
		
		mPreferences.Write(Config::kCutKeyDialogAddr, sizeof(Config::SCutKeyDialogPrefs), (uint8_t*)&prefs);

		SKeySpec& spec = pinsValueField.GetKeySpec();
		//Serial.printf("CutKey = %s, pin count = %hd\n", spec.name, prefs.pinCountMenuItemTag);

		uint32_t	keyHolderOriginX = 0;
		uint32_t	keyHolderOriginZ = 0;
		/*
		*	If the key holder origin has been defined AND
		*	the cutter head isn't busy THEN
		*	move the cutter head to 0.5mm to the right of the key shoulder and
		*	10mm above the bottom of the key holder slot.
		*/
		if (GetKeyHolderOrigin(keyHolderOriginX, keyHolderOriginZ) &&
			HomeAndMoveCutterHeadTo(keyHolderOriginX+50,
									keyHolderOriginZ-1000))
		{
			// Turn on cutter...
			mActionQueue.AppendAction(&mStartMotor);
			// cut the key
			mCutKey.Setup(&spec, prefs.pinCountMenuItemTag, keyHolderOriginX, keyHolderOriginZ, prefs.pinDepths);
			mActionQueue.AppendAction(&mCutKey);
			// Turn off cutter...
			mActionQueue.AppendAction(&mStopMotor);
			// move cutter head out of the way.
			mFastMoveZTo[1].SetSteps(100, false);	// Move to 1mm below endstop
			mActionQueue.AppendAction(&mFastMoveZTo[1]);
			mFastMoveXTo[1].SetSteps(1500, false);	// Move to 15mm before endstop
			mActionQueue.AppendAction(&mFastMoveXTo[1]);
		
	//	} else
	//	{
	// Unable to read prefs or there is something in the action queue.
	// THis should never happen so no error is displayed.
		}
		
		


#if 1
	} else
	{
		Serial.printf("errorCode = %d, pin = %d\n", (uint32_t)errorCode, errorPin);
#endif
	}
}

/****************************** DisableSteppers *******************************/
void KeyMachineSTM32::DisableSteppers(void)
{
	digitalWrite(Config::kXEnablePin, HIGH);
//	digitalWrite(Config::kYEnablePin, HIGH);	Stepper not used
	digitalWrite(Config::kZEnablePin, HIGH);
//	digitalWrite(Config::kEXEnablePin, HIGH);	Stepper not used
	digitalWrite(Config::kSteppersEnabledPin, LOW);
	mSteppersHomed = false;
}
	
/******************************* EnableSteppers *******************************/
void KeyMachineSTM32::EnableSteppers(void)
{
	digitalWrite(Config::kXEnablePin, LOW);
//	digitalWrite(Config::kYEnablePin, LOW);		Stepper not used
	digitalWrite(Config::kZEnablePin, LOW);
//	digitalWrite(Config::kEXEnablePin, LOW);	Stepper not used
	digitalWrite(Config::kSteppersEnabledPin, HIGH);
}
	
/******************************** EndstopsHomed *******************************/
/*
*	Called when the mHomeXEndstop finishes.  mHomeXEndstop is executed after
*	mHomeYEndstop.
*/
void KeyMachineSTM32::EndstopsHomed(
	KMAction*	inAction,
	uint32_t	inExitState)
{
	mSteppersHomed = inExitState == KMAction::eExitNormal;
}

/***************************** SaveKMSettingsToSD *****************************/
void KeyMachineSTM32::SaveKMSettingsToSD(void)
{
	if (digitalRead(Config::kSDDetectPin) == LOW)
	{
		KMSettings	kmSettings;
		SKMSettings	settings;
		settings.hourFormat = UnixTime::Format24Hour() ? 24:12,
		settings.currentView = kInfoViewTag;
		mTouchScreen.GetMinMax(settings.tsMinMax);

		SdFat sd;
		bool	success = sd.begin(Config::kSDSelectPin, SD_SCK_MHZ(4));
		if (success)
		{
			success = kmSettings.WriteFile(kKMSettingsPath, settings);
		} else
		{
			sd.initErrorHalt();
		}
		warningDialog.DoMessage(success ? kSavedKMSettingsStr : kSaveToSDFailedStr);
	} else
	{
		warningDialog.DoMessage(kNoSDCardFoundStr);
	}
}

/**************************** LoadKMSettingsFromSD ****************************/
void KeyMachineSTM32::LoadKMSettingsFromSD(void)
{
	if (digitalRead(Config::kSDDetectPin) == LOW)
	{
		SdFat sd;
		KMSettings	kmSettings;
		bool	success = sd.begin(Config::kSDSelectPin, SD_SCK_MHZ(4));
		if (success)
		{
			success = kmSettings.ReadFile(kKMSettingsPath);
		} else
		{
			sd.initErrorHalt();
		}
		if (success)
		{
			const SKMSettings&	settings = kmSettings.Settings();
			Config::SKMPreferences	prefs = {0};
			prefs.clockFormat = settings.hourFormat == 12;
			memcpy(prefs.tsMinMax, settings.tsMinMax, sizeof(prefs.tsMinMax));
			mPreferences.Write(0, sizeof(Config::SKMPreferences), &prefs.unused[0]);
			/*
			*	Restart the board.
			*/
			HAL_NVIC_SystemReset();
		} else
		{
			warningDialog.DoMessage(kLoadFromSDFailedStr);
		}
	} else
	{
		warningDialog.DoMessage(kNoSDCardFoundStr);
	}
}