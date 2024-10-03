/*
*	STMTimers.h, Copyright Jonathan Mackey 2024
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

#ifndef STMTimers_H
#define STMTimers_H
#include <Arduino.h>
#if defined(STM32F1xx)

#if defined(STM32F103xB)	// stm32f103x8 -> B
#define MAX_TIMERS	7
#elif defined(STM32F103xE)	// stm32f103xC -> E
#define MAX_TIMERS	8
#elif defined(STM32F103xG)	// stm32f103xF -> G
#define MAX_TIMERS	14
#endif

#define DebugSTMTimers	1
class STMTimers
{
public:
	static bool				Allocate(
								TIM_TypeDef*			inTimer);
	static TIM_TypeDef* 	AllocateNextAvailable(void);
	static uint32_t			GetReservedTimers(void)
								{return(sReservedTimers);}
#ifdef DebugSTMTimers
	static void				DumpFrequencies(void);
	static void				ClearFrequencies(void)
								{sFrequenciesIndex = 0;}
	static void				PushFrequency(
								uint32_t				inFrequency);
#endif
protected:
	static uint32_t		sReservedTimers;	// Mask of reserved timers
	static TIM_TypeDef*	sTimers[MAX_TIMERS];
#ifdef DebugSTMTimers
	static uint32_t		sFrequencies[20];
	static uint32_t		sFrequenciesIndex;
#endif
};

#endif	// STM32F1xx
#endif	// STMTimers_H