/*
*	HomeEndstop.cpp, Copyright Jonathan Mackey 2024
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
#include "HomeEndstop.h"
#include "Endstop.h"
#include "Config.h"

const char	HomeEndstop::kName[] = "Home Endstop";

/******************************** HomeEndstop *********************************/
HomeEndstop::HomeEndstop(
	StepControl*	inController,
	Stepper*		inStepper,
	Endstop*		inEndstop,
	int32_t			inDir)
	: mController(inController), mStepper(inStepper), mEndstop(inEndstop),
	  mDir(inDir)
{
	mDir = inDir < 0 ? -1:1;
}

/************************************ begin ***********************************/
void HomeEndstop::begin(void)
{
	mExitState = eActionFailed;
	mCurrentTask = eBegin;
	if (mEndstop->AtEndstop())
	{
		mCurrentTask++;
	}
}

/*********************************** IsDone ***********************************/
bool HomeEndstop::IsDone(void)
{
	bool	done = mCurrentTask >= eDone;
	if (!mController->isRunning() && !done)
	{
		mCurrentTask++;
		switch(mCurrentTask)
		{
			case eFastMoveToEndstop:
				if (!mEndstop->AtEndstop())
				{
					mStepper->setMaxSpeed(TO_MICROSTEPS(1000));			// steps/s
					mStepper->setAcceleration(TO_MICROSTEPS(250));			// steps/s^2 
					mStepper->setTargetRel(TO_MICROSTEPS(20000) * mDir);
				} else
				{
					mExitState = eActionFailed + 1;
					done = true;
				}
				break;
			case eSlowBackoffEndstop:
				if (mEndstop->AtEndstop())
				{
					mStepper->setMaxSpeed(TO_MICROSTEPS(100));				// steps/s
					mStepper->setAcceleration(TO_MICROSTEPS(25));			// steps/s^2 
					mStepper->setTargetRel(TO_MICROSTEPS(-200) * mDir);	// Backup 2mm
				} else
				{
					mExitState = eActionFailed + 2;
					done = true;
				}
				break;
			case eSlowMoveToEndstop:
				if (!mEndstop->AtEndstop())
				{
					mStepper->setTargetRel(TO_MICROSTEPS(400) * mDir);
				} else
				{
					mExitState = eActionFailed + 3;
					done = true;
				}
				break;
			case eDone:
				mStepper->setPosition(0);
				mExitState = mEndstop->AtEndstop() ? eExitNormal : (eActionFailed+4);
				done = true;
				break;
		}
		if (!done)
		{
			mController->moveAsync(*mStepper);
		} else if (mCallback)
		{
			mCallback(this, mExitState);
		}
	}
	return(done);
}

