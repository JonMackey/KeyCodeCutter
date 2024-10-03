/*
*	MCP45X1.h, Copyright Jonathan Mackey 2024
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

#ifndef MCP45X1_H
#define MCP45X1_H

// This class has only been tested with the MCP4531-103E device.

class MCP45X1
{
public:
							MCP45X1(
								uint8_t					inDeviceAddress);
	enum EMemAddress
	{
		eWiper0,
		eWiper1			= 0x0100,	//	N/A this device
		eNVWiper0		= 0x0200,	//	N/A this device
		eNVWiper1		= 0x0300,	//	N/A this device
		eTCONReg		= 0x0400,
		eStatusReg		= 0x0500	//	N/A this device
	};
	enum TCONBits
	{
		eR0B			= 0b00000001,
		eR0W			= 0b00000010,
		eR0A			= 0b00000100,
		eR0HW			= 0b00001000,	//	N/A this device
		eR1B			= 0b00010000,	//	N/A this device
		eR1W			= 0b00100000,	//	N/A this device
		eR1A			= 0b01000000,	//	N/A this device
		eR1HW			= 0b10000000	//	N/A this device
	};
	uint16_t				Read(
								uint8_t					inAddress = eWiper0);	// Read Wiper 0
	bool					Write(
								uint16_t				inValue,
								uint8_t					inAddress = eWiper0);
								
	bool					Increment(void);	// Increment Wiper 0
	bool					Decrement(void);	// Decrement Wiper 0
	bool					Increment1(void);	// Wiper 1, N/A this device
	bool					Decrement1(void);	// Wiper 1, N/A this device

protected:
	uint8_t		mDeviceAddress;	// 0x50 + N low address bit(s).

	enum ECommands
	{
		eWriteData,
		eIncrement0	= 0b0100,
		eDecrement0	= 0b1000,
		eIncrement1	= 0b10100,
		eDecrement1	= 0b11000,
		eReadData	= 0b1100
	};

};

#endif