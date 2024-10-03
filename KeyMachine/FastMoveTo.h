/*
*	FastMoveTo.h, Copyright Jonathan Mackey 2024
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


#ifndef FastMoveTo_h
#define FastMoveTo_h

#include "KMAction.h"

class Endstop;

class FastMoveTo : public KMAction
{
public:
							FastMoveTo(
								StepControl*			inController,
								Stepper*				inStepper);
	void					SetSteps(
								int32_t					inSteps,
								bool					inRelative)
								{mSteps = inSteps; mRelative = inRelative;}
	virtual void			begin(void);
	virtual bool			IsDone(void);
	virtual const char*		Name(void) const
								{return(kName);}

protected:
	enum EFastMoveToTask
	{
		eBegin,
		eFastMoveTo,
		eDone
	};
	StepControl*	mController;
	Stepper*		mStepper;
	int32_t			mSteps;
	uint32_t		mCurrentTask;
	bool			mRelative;
	static const char	kName[];
};

#endif /* FastMoveTo_h */
