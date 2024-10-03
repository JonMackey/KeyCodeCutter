/*
*	KMActionQueue.cpp, Copyright Jonathan Mackey 2024
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
#include "KMActionQueue.h"
#include "KMAction.h"

/******************************** KMActionQueue *******************************/
KMActionQueue::KMActionQueue(void)
	: mHead(nullptr), mTail(nullptr),
	  mCurrent(nullptr), mState(KMActionQueue::eQueueEmpty)
{
}

/******************************** AppendAction ********************************/
void KMActionQueue::AppendAction(
	KMAction*	inAction)
{
	inAction->SetNext(nullptr);
	if (mTail)
	{
		mTail->SetNext(inAction);
		inAction->SetPrev(mTail);
		mTail = inAction;
	} else
	{
		inAction->SetPrev(nullptr);
		mHead = mTail = inAction;
		mState = eActionPending;
	}
}

/****************************** DetachHeadAction ******************************/
KMAction* KMActionQueue::DetachHeadAction(void)
{
	KMAction*	detachedAction = mHead;
	if (detachedAction)
	{
		mHead = detachedAction->Next();
		if (mHead)
		{
			detachedAction->SetNext(nullptr);
			mHead->SetPrev(nullptr);
			mState = eActionPending;
		} else
		{
			mTail = nullptr;
			mState = eQueueEmpty;
		}
		mCurrent = nullptr;
	}
	return(detachedAction);
}

/************************************ Clear ***********************************/
void KMActionQueue::Clear(void)
{
	mHead = mTail = mCurrent = nullptr;
	mState = eQueueEmpty;
}

/******************************* ContinueAction *******************************/
KMActionQueue::EActionQueueState KMActionQueue::ContinueAction(void)
{
	/*
	*	If there is a current action
	*/
	if (mCurrent)
	{
		if (mState == eActionExecuting)
		{
			/*
			*	If the current action finished...
			*/
			if (mCurrent->IsDone())
			{
				/*
				*	If the action exited normally THEN
				*	detach the current action.
				*/
				if (mCurrent->ExitState() == KMAction::eExitNormal)
				{
					DetachHeadAction();
				} else
				{
					/*
					*	When the action fails the host must decide how to
					*	proceed.  In most cases the queue will be emptied.
					*/
					mState = eActionFailed;
				}
			}	// Else it's still executing
		} // Else it failed
	/*
	*	Else if there are any actions left in the queue THEN
	*	load the next action.
	*/
	} else if (mHead)
	{
		mCurrent = mHead;
		mCurrent->begin();
		mState = eActionExecuting;
	}
	return(mState);
}