/*
*	MCP45X1.cpp, Copyright Jonathan Mackey 20124
*	Class to access data on the MCP45X1 7-Bit Single I2C Digital POT.
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

#include "Arduino.h"
#include "MCP45X1.h"
#include <Wire.h>

/*********************************** MCP45X1 ************************************/
MCP45X1::MCP45X1(
	uint8_t	inDeviceAddress)
	: mDeviceAddress(inDeviceAddress)
{
}

/************************************ Read ************************************/
uint16_t MCP45X1::Read(
	uint8_t	inAddress)
{
	Wire.beginTransmission(mDeviceAddress);
	Wire.write(inAddress | eReadData);
	Wire.endTransmission(false);
	
	uint16_t	bytesRead = Wire.requestFrom(mDeviceAddress, 2);
	if (bytesRead == 2)
	{
		uint16_t	msb = (Wire.read() & 1) << 8;
		return(msb | Wire.read());
	}
	return(0xFFFF);	// Invalid for both eWiper0 and eTCONReg
}

/*********************************** Write ************************************/
bool MCP45X1::Write(
	uint16_t	inValue,
	uint8_t		inAddress)
{
	Wire.beginTransmission(mDeviceAddress);
	Wire.write(inAddress | ((inValue >> 8) & 1));
	Wire.write((uint8_t)inValue);
	return(Wire.endTransmission(true) == 0);
}

/********************************** Increment *********************************/
bool MCP45X1::Increment(void)
{
	Wire.beginTransmission(mDeviceAddress);
	Wire.write(eIncrement0);
	return(Wire.endTransmission(true) == 0);
}

/********************************** Decrement *********************************/
bool MCP45X1::Decrement(void)
{
	Wire.beginTransmission(mDeviceAddress);
	Wire.write(eDecrement0);
	return(Wire.endTransmission(true) == 0);
}

/********************************* Increment1 *********************************/
bool MCP45X1::Increment1(void)
{
	Wire.beginTransmission(mDeviceAddress);
	Wire.write(eIncrement1);
	return(Wire.endTransmission(true) == 0);
}

/********************************* Decrement1 *********************************/
bool MCP45X1::Decrement1(void)
{
	Wire.beginTransmission(mDeviceAddress);
	Wire.write(eDecrement1);
	return(Wire.endTransmission(true) == 0);
}
