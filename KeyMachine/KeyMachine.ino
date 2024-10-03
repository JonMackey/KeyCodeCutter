/*
*	KeyMachine.ino, Copyright Jonathan Mackey 2024
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
*/
#include <Arduino.h>
#include <SPI.h>
#include <Wire.h>

#include "KeyMachineSTM32.h"

KeyMachineSTM32	keyMachine;
/*
*	The definition of Serial on non-BluePill boards is by default USART2. This
*	means that Rx/Tx is on PA3/PA2 rather than PA10/PA9.
*	Serial is a define set to Serial2
*/
#if 0
#define xstr(s) str(s)
#define str(s) #s

#pragma message "The value of PA9: " xstr(PA9)
#pragma message "The value of PA2: " xstr(PA2)
#pragma message "The value of PIN_SERIAL_TX: " xstr(PIN_SERIAL_TX)
#pragma message "The value of PA_3: " xstr(PA_3)
#pragma message "The value of PIN_SERIAL_RX: " xstr(PIN_SERIAL_RX)
#endif

/*********************************** setup ************************************/
void setup(void)
{
	Serial.begin(BAUD_RATE);
	Serial.printf("Starting...\n");

	SPI.begin();
	
	// Note that in order for the correct override of STM32's version of
	// Wire.begin to be called, SDAPin and SCLPin must be defined as uint32_t
	Wire.begin(Config::kSDAPin, Config::kSCLPin);	// Using SDA2/SCL2
	keyMachine.begin();
}

/************************************ loop ************************************/
void loop(void)
{
	keyMachine.Update();
}
