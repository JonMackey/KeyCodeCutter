/*
*	Endstop.h, Copyright Jonathan Mackey 2024
*
*	Manages a single endstop on a STM32 platform
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


#ifndef Endstop_h
#define Endstop_h

#include "PlatformDefs.h"
#include "MSPeriod.h"

class Endstop;
typedef std::function<void(Endstop*, bool)> EndstopCallback;

class Endstop
{
public:
							Endstop(
								pin_t					inEndstopPin);
	void					begin(
								EndstopCallback			inCallback = nullptr);
	void					SetCallback(
								EndstopCallback			inCallback)
								{mCallback = inCallback;}
	bool					AtEndstop(void) const
								{return (mAtEndstop);}
	pin_t					GetEndstopPin(void) const
								{return(mEndstopPin);}
	uint32_t				GetPortPinMask(void) const
								{return(mPortPinMask);}
protected:
	bool				mAtEndstop;
	uint32_t			mPortPinMask;
	pin_t				mEndstopPin;
	GPIO_TypeDef*		mGPIOPort;
	EndstopCallback		mCallback;
	MSPeriod			mDebouncePeriod;
	
	void					EndstopChangedISR(void);
};
#endif /* Endstop_h */
