/*
*	KMActionQueue.h, Copyright Jonathan Mackey 2024
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


#ifndef KMActionQueue_h
#define KMActionQueue_h

class KMAction;

class KMActionQueue
{
public:
							KMActionQueue(void);
	void					AppendAction(
								KMAction*				inKMAction);
	KMAction*				DetachHeadAction(void);
	void					Clear(void);
	KMAction*				Head(void) const
								{return(mHead);}
	KMAction*				Tail(void) const
								{return(mTail);}
	KMAction*				Current(void) const
								{return(mCurrent);}
	enum EActionQueueState
	{
		eQueueEmpty,
		eActionPending,
		eActionExecuting,
		eActionFailed
	};
	EActionQueueState		ContinueAction(void);
	EActionQueueState		State(void)
								{return(mState);}
	bool					IsEmpty(void) const
								{return(mState == eQueueEmpty);}
protected:
	KMAction*	mHead;
	KMAction*	mTail;	
	KMAction*	mCurrent;
	EActionQueueState	mState;	
};

#endif /* KMActionQueue_h */
