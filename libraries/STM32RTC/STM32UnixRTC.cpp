/*
*	STM32UnixRTC.cpp, Copyright Jonathan Mackey 2022
*
*	Class to manage the date and time for an STM32.
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
#include "STM32UnixRTC.h"


#ifndef __MACH__
#include "Arduino.h"
#include "MSPeriod.h"
#if defined(STM32_CORE_VERSION) && (STM32_CORE_VERSION  > 0x01090000)
	#include "rtc.h"
#endif
// Check if RTC HAL enable in variants/board_name/stm32yzxx_hal_conf.h
#ifndef HAL_RTC_MODULE_ENABLED
	#error "RTC configuration is missing. Check flag HAL_RTC_MODULE_ENABLED in variants/board_name/stm32yzxx_hal_conf.h"
#endif

// 0x4000 7000 - 0x4000 73FF Power control PWR
// uint32_t* const 	kPWR_CR = ((uint32_t*) 0x40007000);

// 0x4000 2800 - 0x4000 2BFF RTC
uint32_t* const 	kRTC_CRL = ((uint32_t*)0x40002804);
uint32_t* const 	kRTC_CNTH = ((uint32_t*)0x40002818);
uint32_t* const 	kRTC_CNTL = kRTC_CNTH+1;

/********************************** RTCInit ***********************************/
void STM32UnixRTC::RTCInit(void)
{
	// LSE = low-speed external 32.768 kHz crystal
	// RTC_init will only fully initialize if the RTC has never been initialized.
	// The RTC_init writes a marker to the backup memory to determine if it
	// needs to be initialized.  So it's safe to call on reset/startup.
	if (RTC_init(HOUR_FORMAT_24, LSE_CLOCK, false))
	{
		sTime = 0x6423FFF0;
		SyncRTCToTime();
	}

	sTime = ReadRTCCount();
	attachSecondsIrqCallback(SecondsCB);
	ResetSleepTime();
#if 0
	Serial.print("CNTH = 0x");
	Serial.print(*kRTC_CNTH, HEX);
	Serial.print(", CNTL = 0x");
	Serial.println(*kRTC_CNTL, HEX);
#endif
}

/********************************** SyncTime **********************************/
void STM32UnixRTC::SyncRTCToTime(void)
{
#if 0
	Serial.print("BF CNTH = 0x");
	Serial.print(*kRTC_CNTH, HEX);
	Serial.print(", CNTL = 0x");
	Serial.println(*kRTC_CNTL, HEX);
	Serial.print("RTOFF = ");
	Serial.println((*kRTC_CRL & RTC_CRL_RTOFF), HEX);
#endif
	//sTime = 1680566385; // 15 seconds before midnight test
	//sTime = 0x6423FFF0; // 15 seconds till overflow kRTC_CNTL test
	bool success = WriteRTCCount(sTime);
#if 0
	Serial.print("WriteRTCCount(");
	Serial.print(sTime, HEX);
	Serial.println(success ? ") success" : ") failed");
	Serial.print("CNTH = 0x");
	Serial.print(*kRTC_CNTH, HEX);
	Serial.print(", CNTL = 0x");
	Serial.println(*kRTC_CNTL, HEX);
#endif
}

/********************************* SecondsCB **********************************/
void STM32UnixRTC::SecondsCB(
	void*	inUserData)
{
	UnixTime::Tick();
}

/******************************** ReadRTCCount ********************************/
uint32_t STM32UnixRTC::ReadRTCCount(void)
{
	uint32_t	countH = *kRTC_CNTH;
	uint32_t	countL = *kRTC_CNTL;
	uint32_t	countH2 = *kRTC_CNTH;
	/*
	*	The STM code checks to see if while reading kRTC_CNTH the value changed.
	*	If it is, then re-read kRTC_CNTL
	*/
	if (countH != countH2)
	{
		countL = *kRTC_CNTL;
	}
	return((countH2 << 16) + (countL & 0xFFFF));
}

/******************************** WriteRTCCount *******************************/
bool STM32UnixRTC::WriteRTCCount(
	uint32_t	inSeconds)
{
	MSPeriod	timeout(1000);
	bool		success = true;

	timeout.Start();
	
	/*
	*	Wait till any previous RTC write operation finishes
	*/
	while ((*kRTC_CRL & RTC_CRL_RTOFF) == 0)
	{
		if (!timeout.Passed())continue;
		success = false;
		break;
	}
	if (success)
	{
		SET_BIT(*kRTC_CRL, RTC_CRL_CNF);	// Disable write protection
		delay(1); // Without this delay the write to RTC_CNTL and RTC_CNTH doesn't happen (no change)
		*kRTC_CNTL = (inSeconds & 0xFFFF);	// Write the low 16 bits
		*kRTC_CNTH = (inSeconds>>16);		// Write the high 16 bits
		CLEAR_BIT(*kRTC_CRL, RTC_CRL_CNF);	// Enable write protection
		
		timeout.Start();
		/*
		*	Wait till this RTC write operation finishes
		*/
		while ((*kRTC_CRL & RTC_CRL_RTOFF) == 0)
		{
			if (!timeout.Passed())continue;
			success = false;
			break;
		}
	}
	return(success);
}

#endif	// __MACH__

