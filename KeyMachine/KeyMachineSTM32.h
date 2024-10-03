/*
*	KeyMachineSTM32.h, Copyright Jonathan Mackey 2024
*	- Monitors the dust collector to determine when the drum or filter is full.
*	- Controls the drum motor and warning flasher.
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
#ifndef KeyMachineSTM32_h
#define KeyMachineSTM32_h

#include "Config.h"
#include "AT24C.h"
#include "TFT_ILI9488.h"
#include "XPT2046.h"
#include "XDialogBox.h"
#include "MSPeriod.h"
//#include "USPeriod.h"
#include "STM32UnixRTC.h"
#include "MCP45X1.h"
#include "TeensyStep.h"
#include "Endstop.h"
#include "HomeEndstop.h"
#include "FastMoveTo.h"
#include "CutKey.h"
#include "CallbackAction.h"
#include "KMActionQueue.h"

class KeyMachineSTM32 : public XViewChangedDelegate,
								public XValidatorDelegate
{
public:
							KeyMachineSTM32(void);
		
	virtual void			begin(void);
	virtual bool			Update(void);

	virtual void			HandleViewChange(
								XView*					inView,
								uint16_t				inAction);
	virtual bool			ValuesAreValid(
								XDialogBox*				inDialog);
								
protected:
	XView*			mHitView;
	TFT_ILI9488		mDisplay;
	XPT2046			mTouchScreen;
	AT24C			mPreferences;
	MCP45X1			mPOT;
	StepControl		mController;
	Stepper			mStepperX;
	Stepper			mStepperZ;
	Endstop			mXMinEndstop;
	Endstop			mXMaxEndstop;
	Endstop			mZMinEndstop;
	Endstop			mZMaxEndstop;
	Endstop*		mEndstopChanged;
	KMActionQueue	mActionQueue;
	uint32_t		mLastActionQueueState;
	HomeEndstop		mHomeXEndstop;
	HomeEndstop		mHomeZEndstop;
	FastMoveTo		mFastMoveXTo[2];
	FastMoveTo		mFastMoveZTo[2];
	CutKey			mCutKey;
	CallbackAction	mStopMotor;
	CallbackAction	mStartMotor;
	bool			mMotorIsRunning;
	bool			mDisplaySleeping;
	bool			mSteppersHomed;
	bool			mButtonPressed;
	bool			mEmergencyBtnPressed;
	bool			mMCP45X1Exists;
	MSPeriod		mButtonDebouncePeriod;
	MSPeriod		mEmergencyButtonDebouncePeriod;
	uint32_t		mButtonPinState;
	uint16_t		mX, mY;
	
protected:
	void					GiveTimeToActions(void);
	void					UpdateEndstops(void);
	bool					NoModalDialogDisplayed(void) const;
	void					ShowInfoView(void);
	void					ShowUtilitiesDialog(void);
	void					ShowAdjustOriginDialog(void);
	void					SaveAdjustOriginDialogChanges(void);
	void					ShowSetupZeroDialog(void);
	void					SaveSetupKeyHolderDialogChanges(void);
	bool					HomeAndMoveCutterHeadTo(
								int32_t					inX,
								int32_t					inZ);
	bool					GetKeyHolderOrigin(
								uint32_t&				outOriginX,
								uint32_t&				outOriginY);
	void					ShowCutKeyDialog(void);
	void					LoadKeySpecByTag(
								uint16_t				inTag,
								bool					inUpdate = true);
	const SKeySpec*			FindKeySpecByTag(
								uint16_t				inTag) const;
	void					DoCutKey(void);
	void					SaveKMSettingsToSD(void);
	void					LoadKMSettingsFromSD(void);
	void					UpdateInfoView(void);
	void					WakeUp(void);
	void					GoToSleep(void);
	void					EndstopChangedISR(
								Endstop*				inEndstop,
								bool					inAtEndstop);
	void					EndstopsHomed(
								KMAction*				inAction,
								uint32_t				inExitState);
	void					EmergencyStopISR(void);
	void					ButtonPressedISR(void);
	void					CheckButtons(void);
	void					StopSteppers(void);
	void					StartKMMotor(
								KMAction*				inAction = nullptr,
								uint32_t				inExitState = 0);
	void					StopKMMotor(
								KMAction*				inAction = nullptr,
								uint32_t				inExitState = 0);
	void					DisableSteppers(void);
	void					EnableSteppers(void);
};

#endif // KeyMachineSTM32_h
