/*
*	KMAction.h, Copyright Jonathan Mackey 2024
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


#ifndef KMAction_h
#define KMAction_h

#ifdef __MACH__
#include <inttypes.h>
#include <stdio.h>
#include <functional>
#else
#include <Arduino.h>
#include "TeensyStep.h"
#endif


class KMAction;
typedef std::function<void(KMAction*, uint32_t)> KMActionCallback;

class KMAction
{
public:
							KMAction(void)
							: mCallback(nullptr){}
	virtual void			begin(void) = 0;
	virtual bool			IsDone(void) = 0;
	virtual const char*		Name(void) const = 0;
	uint32_t				ExitState(void) const
								{return(mExitState);}
	KMAction*				Prev(void) const
								{return(mPrev);}
	void					SetPrev(
								KMAction*				inPrev)
								{mPrev = inPrev;}
	KMAction*				Next(void) const
								{return(mNext);}
	void					SetNext(
								KMAction*				inNext)
								{mNext = inNext;}
	void					SetCallback(
								KMActionCallback		inCallback)
								{mCallback = inCallback;}
	enum EExitState
	{
		eExitNormal,
		eActionFailed
	};
protected:
	KMAction*			mPrev;
	KMAction*			mNext;	
	uint32_t			mExitState;
	KMActionCallback	mCallback;
};

#endif /* KMAction_h */
