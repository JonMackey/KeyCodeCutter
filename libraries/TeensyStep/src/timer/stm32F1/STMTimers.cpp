/*
*	STMTimers.cpp, Copyright Jonathan Mackey 2024
*	Static class to manage the pool of STM32 timers.
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

#if defined(STM32F1xx)
#include "STMTimers.h"

uint32_t STMTimers::sReservedTimers = 0;

#ifdef DebugSTMTimers
uint32_t STMTimers::sFrequenciesIndex = 0;
uint32_t STMTimers::sFrequencies[20];
#endif

/*
*	The timers below were derived from the respective documentation section
*	"Timers and watchdogs".	 You can also get them from the STM32 package
*	headers: stm32f103xb.h, stm32f103xe.h, stm32f103xg.h, then search for
*	"Peripheral_declaration"
*/
#if defined(STM32F103xB)	// stm32f103xB -> D
// MAX_TIMERS = 4
TIM_TypeDef* STMTimers::sTimers[MAX_TIMERS] = { TIM1, TIM2, TIM3, TIM4};
#elif defined(STM32F103xE)	// stm32f103xE
// MAX_TIMERS = 8
TIM_TypeDef* STMTimers::sTimers[MAX_TIMERS] = { TIM1, TIM2, TIM3, TIM4,
													TIM5, TIM6, TIM7, TIM8};
#elif defined(STM32F103xG)	// stm32f103xF -> G
// MAX_TIMERS = 14
TIM_TypeDef* STMTimers::sTimers[MAX_TIMERS] = { TIM1, TIM2, TIM3, TIM4,
												TIM5, TIM6, TIM7, TIM8,
												TIM9, TIM10, TIM11, TIM12,
												TIM13, TIM14};
#else
#pragma GCC error "No valid STM32 target defined"
#endif

#ifdef DebugSTMTimers
/****************************** DumpFrequencies *******************************/
void STMTimers::DumpFrequencies(void)
{
	for (uint32_t i = 0; i < sFrequenciesIndex; i++)
	{
		Serial.printf("f[%d] = %d\n", i, sFrequencies[i]);
	}
}

/******************************** PushFrequency *******************************/
void STMTimers::PushFrequency(
	uint32_t	inFrequency)
{
	if (sFrequenciesIndex < 20)
	{
		sFrequencies[sFrequenciesIndex++] = inFrequency;
	}
}
#endif
/********************************** Allocate **********************************/
/*
*	Attempts to allocate a timer from the pool of timers.
*	True is returned if the timer exists and is not already reserved.
*/
bool STMTimers::Allocate(
	TIM_TypeDef*	inTimer)
{
	for (uint32_t i = 0; i < MAX_TIMERS; i++)
	{
		if (sTimers[i] != inTimer)
		{
			continue;
		}
		uint32_t	timerMask = 1<<i;
		if ((sReservedTimers & timerMask) == 0)
		{
			sReservedTimers |= timerMask;
			return(true);
		}
		break;
	}
	return(false);
}
/**************************** AllocateNextAvailable ***************************/
/*
*	Attempts to allocate the next available timer from the pool of timers.
*	Returns the timer pointer if available, else nul is returned.
*/
TIM_TypeDef* STMTimers::AllocateNextAvailable(void)
{
	uint32_t	timerMask = 1;
	for (uint32_t i = 0; i < MAX_TIMERS; i++, timerMask <<= 1)
	{
		if (sReservedTimers & timerMask)
		{
			continue;
		}
		sReservedTimers |= timerMask;
		return(sTimers[i]);
	}
	return(nullptr);
}
#endif	// STM32F1xx
