/*
*	CallbackAction.cpp, Copyright Jonathan Mackey 2024
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
#include "CallbackAction.h"
#ifndef __MACH__
#include "Config.h"
#else
#include <string>
#endif

const char	CallbackAction::kName[] = "Callback";

/*********************************** CallbackAction ***********************************/
CallbackAction::CallbackAction(void)
{
}

/******************************** SetWaitPeriod *******************************/
void CallbackAction::SetWaitPeriod(
	uint32_t	inWaitBeforeCallback,
	uint32_t	inWaitAfterCallback)
{
	mWaitBeforeCallback.Set(inWaitBeforeCallback);
	mWaitAfterCallback.Set(inWaitAfterCallback);
}

/************************************ begin ***********************************/
void CallbackAction::begin(void)
{
	mExitState = eActionFailed;
	mCurrentTask = eCallback;
	/*
	*	If there's a delay before the callback THEN
	*	start the timer
	*/
	if (mWaitBeforeCallback.Get())
	{
		mWaitBeforeCallback.Start();
		mCurrentTask = eWaitingBefore;
	}
}

/*********************************** IsDone ***********************************/
bool CallbackAction::IsDone(void)
{
	bool	done = false;
	switch(mCurrentTask)
	{
		case eWaitingBefore:
			/*
			*	If still waiting THEN
			*	just return
			*/
			if (!mWaitBeforeCallback.Passed())
			{
				break;
			}	// done waiting, fall through, execute the callback
		case eCallback:
			/*
			*	If there is a callback THEN
			*	call it
			*/
			if (mCallback)
			{
				mCallback(this, mExitState);
			}
			/*
			*	If there's a delay after the callback THEN
			*	start the timer
			*/
			if (mWaitAfterCallback.Get())
			{
				mWaitAfterCallback.Start();
				mCurrentTask = eWaitingAfter;
				break;
			}
			mExitState = eExitNormal;
			done = true;
			break;
		case eWaitingAfter:
			/*
			*	If still waiting THEN
			*	just return
			*/
			if (!mWaitAfterCallback.Passed())
			{
				break;
			}
			mExitState = eExitNormal;
			done = true;
			break;
	}
	return(done);
}

