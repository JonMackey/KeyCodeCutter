/*
*	FastMoveTo.cpp, Copyright Jonathan Mackey 2024
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
#include "FastMoveTo.h"
#include "Config.h"

const char	FastMoveTo::kName[] = "Fast move to";

/****************************** FastMoveTo *****************************/
FastMoveTo::FastMoveTo(
	StepControl*	inController,
	Stepper*		inStepper)
	: mController(inController), mStepper(inStepper), mSteps(0), mRelative(false)
{
}

/************************************ begin ***********************************/
void FastMoveTo::begin(void)
{
	mExitState = eActionFailed;
	mCurrentTask = eBegin;
	int32_t	position = TO_STEPS(mStepper->getPosition());
	if (!mRelative)
	{
		mSteps -= position;
	}
	/*
	*	If moving towards the endstop AND
	*	the number of steps of the move will finish up within 10 steps of the
	*	end stop, THEN
	*	clip the number of steps to stop at 10
	*/
	if (mSteps < 0 &&
		-mSteps > (position-10))
	{
		if (position > 10)
		{
			mSteps = 10 - position;
		} else
		{
			mCurrentTask = eDone-1;	// will immediately change to eDone in IsDone()
		}
	}
}

/*********************************** IsDone ***********************************/
bool FastMoveTo::IsDone(void)
{
	bool	done = mCurrentTask >= eDone;
	if (!mController->isRunning() && !done)
	{
		mCurrentTask++;
		switch(mCurrentTask)
		{
			case eFastMoveTo:
				mStepper->setMaxSpeed(TO_MICROSTEPS(1000));			// steps/s
				mStepper->setAcceleration(TO_MICROSTEPS(250));			// steps/s^2
				mStepper->setTargetRel(TO_MICROSTEPS(mSteps));
				mController->moveAsync(*mStepper);
				break;
			case eDone:
				mExitState = eExitNormal;
				done = true;
				break;
		}
	}
	return(done);
}

