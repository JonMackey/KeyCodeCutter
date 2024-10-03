/*
*	Endstop.cpp, Copyright Jonathan Mackey 2024
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
#include "Endstop.h"

/********************************** Endstop ***********************************/
Endstop::Endstop(
	pin_t	inEndstopPin)
	: mEndstopPin(inEndstopPin), mDebouncePeriod(2)
{
}

/************************************ begin ***********************************/
void Endstop::begin(
	EndstopCallback	inCallback)
{
	mCallback = inCallback;
	pinMode(mEndstopPin, INPUT);
	attachInterrupt(digitalPinToInterrupt(mEndstopPin),
							std::bind(&Endstop::EndstopChangedISR, this), CHANGE);
	mPortPinMask = digitalPinToBitMask(mEndstopPin);
	mGPIOPort = digitalPinToPort(mEndstopPin);
	mAtEndstop = ((~mGPIOPort->IDR) & mPortPinMask) != 0;
}

/****************************** EndstopChangedISR *****************************/
void Endstop::EndstopChangedISR(void)
{
	/*
	*	React immediately to the first change, then don't react to any
	*	additional changes till after a slight delay to avoid switch bounce.
	*
	*	Note that MSPeriod calls millis().  This is OK because it's only called
	*	once.  The goal is to not call this routine more than once in the same
	*	millisecond.
	*/
	if (mDebouncePeriod.Passed())
	{
		mAtEndstop = ((~mGPIOPort->IDR) & mPortPinMask) != 0;
		if (mCallback)
		{
			mCallback(this, mAtEndstop);
		}
		mDebouncePeriod.Start();
	}
}

