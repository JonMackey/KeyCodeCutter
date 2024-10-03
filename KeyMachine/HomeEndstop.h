/*
*	HomeEndstop.h, Copyright Jonathan Mackey 2024
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


#ifndef HomeEndstop_h
#define HomeEndstop_h

#include "KMAction.h"

class Endstop;

class HomeEndstop : public KMAction
{
public:
							HomeEndstop(
								StepControl*			inController,
								Stepper*				inStepper,
								Endstop*				inEndstop,
								int32_t					inDir);
	virtual void			begin(void);
	virtual bool			IsDone(void);
	virtual const char*		Name(void) const
								{return(kName);}

protected:
	enum EHomeEndstopTask
	{
		eBegin,
		eFastMoveToEndstop,
		eSlowBackoffEndstop,
		eSlowMoveToEndstop,
		eDone
	};
	StepControl*	mController;
	Stepper*		mStepper;
	Endstop*		mEndstop;
	int32_t			mDir;
	uint32_t		mCurrentTask;
	static const char	kName[];
};

#endif /* HomeEndstop_h */
