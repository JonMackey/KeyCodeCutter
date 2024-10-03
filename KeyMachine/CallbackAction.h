/*
*	CallbackAction.h, Copyright Jonathan Mackey 2024
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


#ifndef CallbackAction_h
#define CallbackAction_h

#include "KMAction.h"
#include "MSPeriod.h"

class CallbackAction : public KMAction
{
public:
							CallbackAction(void);
	virtual void			begin(void);
	virtual bool			IsDone(void);
	virtual const char*		Name(void) const
								{return(kName);}
	void					SetWaitPeriod(
								uint32_t				inWaitBeforeCallback,
								uint32_t				inWaitAfterCallback);
protected:
	enum ECallbackTask
	{
		eWaitingBefore,
		eCallback,
		eWaitingAfter
	};
	MSPeriod	mWaitBeforeCallback;
	MSPeriod	mWaitAfterCallback;
	uint32_t	mCurrentTask;
	static const char	kName[];
};

#endif /* CallbackAction_h */
