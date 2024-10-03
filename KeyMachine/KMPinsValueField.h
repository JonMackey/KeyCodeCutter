/*
*	KMPinsValueField.h, Copyright Jonathan Mackey 2024
*	The pin number value fields that can be associated with a stepper.
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
#ifndef KMPinsValueField_h
#define KMPinsValueField_h

#include "XValueField.h"
#include "KeySpec.h"

class KMPinsValueField : public XValueField
{
public:
							KMPinsValueField(
								int16_t					inX,
								int16_t					inY,
								uint16_t				inWidth,
								uint16_t				inTag,
								XView*					inNextView = nullptr,
								XFont::Font*			inFont = nullptr,
								uint16_t				inFGColor = XFont::eWhite,
								uint16_t				inBGColor =  XFont::eBlack);
	virtual void			DrawSelf(void);
	virtual void			MouseDown(
								int16_t					inGlobalX,
								int16_t					inGlobalY);
	virtual void			MouseUp(
								int16_t					inGlobalX,
								int16_t					inGlobalY);
	SKeySpec&				GetKeySpec(void)
								{return(mSpec);}
	void					SetKeySpec(
								const SKeySpec*			inSpec,
								bool					inUpdate = true);
	void					SetPinCount(
								uint32_t				inPinCount,
								bool					inUpdate = true);
	uint16_t				GetPinCount(void) const
								{return(mPinCount);}
	virtual bool			IncrementValue(void);
	virtual bool			DecrementValue(void);
	virtual void			ValueChanged(
								bool					inUpdate = true);
	SKeySpec::EErrorCode	ValuesAreValid(void) const;
	int32_t					GetActivePinDepth(void) const;
	void					SetActivePinDepth(
								int32_t					inCustomDepth,
								bool					inUpdate = true);
	SKeySpec::EErrorCode	GetPinDepthsDec22mm(
								int32_t					outPinArray[],
								uint32_t*				outErrorPin = nullptr) const;
	void					SetPinDepthsDec22mm(
								int32_t					inPinArray[]);
protected:
	enum EPinSubField
	{
		ePin1Field,
		ePin2Field,
		ePin3Field,
		ePin4Field,
		ePin5Field,
		ePin6Field,
		eNumSubFields,
		eNoSubField
	};
	SKeySpec		mSpec;
	uint32_t		mActivePin;
	uint32_t		mDirtyPin;
	uint16_t		mFieldX[eNumSubFields];
	uint16_t		mFieldWidth;
	const char*		mSubFieldStrs[eNumSubFields];
	uint8_t			mPins[SKeySpec::eMaxPinCount];
	uint32_t		mMaxPinIndex;
	uint32_t		mMinPinIndex;
	uint32_t		mPinCount;
	int32_t			mCustomCutDepths[SKeySpec::eMaxPinCount];

	virtual bool			ValueIsValid(
								int32_t					inValue);
	virtual void			UpdateStringForValue(void);
	int32_t					ValueFromPins(void) const;
	bool					SetValueFromPins(
								bool					inUpdate = true);
	void					DrawFields(void);
	void					KeySpecChanged(
								bool					inUpdate);
};
#endif // KMPinsValueField_h
