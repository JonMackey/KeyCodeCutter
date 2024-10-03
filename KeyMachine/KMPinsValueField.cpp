/*
*	KMPinsValueField.cpp, Copyright Jonathan Mackey 2024
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

#include "KMPinsValueField.h"
#include "DisplayController.h"

#ifdef __MACH__
#define _BV(bit) (1 << (bit))
#endif

const char kUnusedPinDepthStr[] = "X";
const char kCustomPinDepthStr[] = "C";
const char* kDecStrs[] = {"0", "1", "2", "3", "4", "5", "6", "7", "8", "9"}; 
const uint16_t	kDarkBlueColor = 0xFB00; // Dark blue
					
/****************************** KMPinsValueField ******************************/
KMPinsValueField::KMPinsValueField(
	int16_t			inX,		// Not used when aligned to stepper
	int16_t			inY,		// Not used when aligned to stepper
	uint16_t		inWidth,
	uint16_t		inTag,
	XView*			inNextView,
	XFont::Font*	inFont,
	uint16_t		inFGColor,
	uint16_t		inBGColor)
	: XValueField(inX, inY, inWidth, inTag, inNextView, inFont, 0, inFGColor, inBGColor),
	  mActivePin(eNoSubField), mCustomCutDepths{}
{
	mSpec.tag = 0;
	XFont*	xFont = MakeFontCurrent();
	if (xFont)
	{
		/*
		*	Initialize the ranges of the date sub fields
		*/
		xFont->LoadGlyph('0');
		uint16_t	digit00Width = xFont->Glyph().advanceX;
		digit00Width *= 2;
		xFont->LoadGlyph(' ');
		uint16_t	spaceWidth = xFont->Glyph().advanceX;

		mFieldWidth = digit00Width;

		uint16_t	fieldX = spaceWidth; // gap
		for (uint16_t i = 0; i < eNumSubFields; i++)
		{
			mFieldX[i] = fieldX;
			fieldX += mFieldWidth;
		}
		fieldX += spaceWidth; // gap
		mWidth = fieldX;
	}
}

/********************************* SetKeySpec *********************************/
void KMPinsValueField::SetKeySpec(
	const SKeySpec*	inSpec,
	bool			inUpdate)
{
	if (inSpec)
	{
		mSpec = *inSpec;
		KeySpecChanged(inUpdate);
	}
}

/******************************** SetPinCount *********************************/
void KMPinsValueField::SetPinCount(
	uint32_t	inPinCount,
	bool		inUpdate)
{
	if (mPinCount != inPinCount)
	{
		mPinCount = inPinCount;
		KeySpecChanged(inUpdate);
	}
}

/******************************* KeySpecChanged *******************************/
void KMPinsValueField::KeySpecChanged(
	bool	inUpdate)
{
	if (mSpec.increasingDepths)
	{
		mMaxPinIndex = mSpec.deepestCutIndex;
		mMinPinIndex = mSpec.shalowestCutIndex;
	} else
	{
		mMaxPinIndex = mSpec.shalowestCutIndex;
		mMinPinIndex = mSpec.deepestCutIndex;
	}
	uint8_t	midIndex = (mMaxPinIndex/2)+1;
	const char*	midIndexStr = kDecStrs[midIndex];
	uint32_t i = 0;
	for (; i < mPinCount; i++)
	{
		mPins[i] = midIndex;
		mSubFieldStrs[i] = midIndexStr;
		mCustomCutDepths[i] = 0;
	}
	for (; i < eNumSubFields; i++)
	{
		mPins[i] = 0;
		mSubFieldStrs[i] = kUnusedPinDepthStr;
	}
	mValue = ValueFromPins();
	mActivePin = 0;
	HandleChange(this, XControl::eOff);
	if (inUpdate)
	{
		DrawSelf();
	}
}

/********************************** DrawSelf **********************************/
void KMPinsValueField::DrawSelf(void)
{
	mDirtyPin = 0x3F;	// Draw all fields
	DrawFields();
}

/********************************* DrawFields *********************************/
void KMPinsValueField::DrawFields(void)
{
	XFont*	xFont = MakeFontCurrent();
	if (xFont)
	{
		DisplayController*	display = xFont->GetDisplay();
		display->SetFGColor(kDarkBlueColor);
		display->SetBGColor(mBGColor);
		int16_t	x = 0;
		int16_t	y = 0;
		// Note: if mY's global value is zero, FillRect and
		// DrawRoundedRect will fail.
		LocalToGlobal(x, y);
		uint32_t	mask = 1;
		for (uint32_t i = 0; i < eNumSubFields; i++, mask <<= 1)
		{
			if (mDirtyPin & mask)
			{
				if (mStepper)
				{
					if (i == mActivePin)
					{
						xFont->SetTextColor(XFont::eWhite);
						xFont->SetBGTextColor(kDarkBlueColor);
						display->DrawRoundedRect(x+mFieldX[i], y-3, mFieldWidth, mHeight+4, 3);
					} else
					{
						xFont->SetTextColor(i < mPinCount ? kDarkBlueColor : XFont::eGray);
						xFont->SetBGTextColor(mBGColor);
						display->FillRect(x+mFieldX[i], y-3, mFieldWidth, mHeight+4, mBGColor);
					}
					xFont->DrawAligned(mSubFieldStrs[i], x+mFieldX[i], y, mFieldWidth,
															XFont::eAlignCenter, false);
				}
			}
		}
		mDirtyPin = 0;
	}
}

/********************************* MouseDown **********************************/
void KMPinsValueField::MouseDown(
	int16_t	inGlobalX,
	int16_t	inGlobalY)
{
	/*
	*	If there is an associated Stepper THEN
	*	there can be an active field.
	*/
	if (mStepper)
	{
		int16_t	x = inGlobalX;
		int16_t	y = inGlobalY;
		GlobalToLocal(x, y);
		uint32_t	i = 0;
		for (; i < mPinCount; i++)
		{
			if (x < mFieldX[i] || x >= mFieldX[i]+mFieldWidth)
			{
				continue;
			}
			break;
		}
		/*
		*	If a valid field was selected AND
		*	it's not already selected
		*/
		if (i < mPinCount &&
			i != mActivePin)
		{
			uint32_t	oldActiveSubfield = mActivePin;
			mActivePin = i;
			if (oldActiveSubfield != eNoSubField)
			{
				mDirtyPin |= _BV(oldActiveSubfield);
			}
			mDirtyPin |= _BV(i);
			DrawFields();
		}
	}
}

/********************************** MouseUp ***********************************/
void KMPinsValueField::MouseUp(
	int16_t	inGlobalX,
	int16_t	inGlobalY)
{
	if (mActivePin < mPinCount)
	{
		HandleChange(this, XControl::eOff);
	}
}

/******************************** ValueIsValid ********************************/
bool KMPinsValueField::ValueIsValid(
	int32_t	inValue)
{
	return(true);
}

/******************************* ValuesAreValid *******************************/
SKeySpec::EErrorCode KMPinsValueField::ValuesAreValid(void) const
{
	int32_t	pinDepths[SKeySpec::eMaxPinCount+1];
	return(GetPinDepthsDec22mm(pinDepths));
}

/**************************** GetPinDepthsDec22mm *****************************/
SKeySpec::EErrorCode KMPinsValueField::GetPinDepthsDec22mm(
	int32_t		outPinArray[],
	uint32_t*	outErrorPin) const
{
	return(mSpec.PinCodeToDec22mm(mValue, mPinCount, mCustomCutDepths, outPinArray, outErrorPin));
}

/**************************** SetPinDepthsDec22mm *****************************/
/*
*	Should only be called when the host dialog isn't showing AND the mSpec 
*	and mPinCount have been initialized.
*/
void KMPinsValueField::SetPinDepthsDec22mm(
	int32_t	inPinArray[])
{
	for (mActivePin = 0; mActivePin < mPinCount; mActivePin++)
	{
		SetActivePinDepth(inPinArray[mActivePin], false);
	}
	SetValueFromPins(false);
	mActivePin = 0;
}

/******************************* IncrementValue *******************************/
bool KMPinsValueField::IncrementValue(void)
{
	uint8_t	oldPinIndex = mPins[mActivePin];
	mPins[mActivePin]++;
	if (mPins[mActivePin] > mMaxPinIndex)
	{
		mPins[mActivePin] = mMinPinIndex;
	}
	mCustomCutDepths[mActivePin] = 0; // Cancel custom value
	int32_t	newValue = ValueFromPins();
	mPins[mActivePin] = oldPinIndex;	// To force the string to update
	return(SetValue(newValue, true));
}

/******************************* DecrementValue *******************************/
bool KMPinsValueField::DecrementValue(void)
{
	uint8_t	oldPinIndex = mPins[mActivePin];
	if (oldPinIndex > mMinPinIndex)
	{
		mPins[mActivePin]--;
	} else
	{
		mPins[mActivePin] = mMaxPinIndex;
	}
	mCustomCutDepths[mActivePin] = 0;	// Cancel custom value
	int32_t	newValue = ValueFromPins();
	mPins[mActivePin] = oldPinIndex;	// To force the string to update
	return(SetValue(newValue, true));
}

/******************************** ValueChanged ********************************/
void KMPinsValueField::ValueChanged(
	bool	inUpdate)
{
	UpdateStringForValue();
	
	if (inUpdate)
	{
		DrawFields();
	}
}

/****************************** SetValueFromPins ******************************/
bool KMPinsValueField::SetValueFromPins(
	bool	inUpdate)
{
	return(SetValue(ValueFromPins(), inUpdate));
}

/****************************** GetActivePinDepth *****************************/
/*
*	Returns the active pin depth as int32 dec22
*/
int32_t KMPinsValueField::GetActivePinDepth(void) const
{
	return(mCustomCutDepths[mActivePin] == 0 ?
				mSpec.IndexToDec22mm(mPins[mActivePin]) :
				mCustomCutDepths[mActivePin]);
}

/******************************** ValueFromPins *******************************/
int32_t KMPinsValueField::ValueFromPins(void) const
{
	int32_t	value = 0;
	for (int32_t i = 0; i < mPinCount; i++)
	{
		value *= 10;
		value += mPins[i];
	}
	return(value);
}

/****************************** SetActivePinDepth *****************************/
/*
*	Sets or cancels a custom pin depth of the active field (pin.)
*	When set the pin depth index changes from a number to a 'C'.
*	When cancelled (inCustomDepth = 0), the pin depth index reverts to the last
*	number displayed for this pin.
*/
void KMPinsValueField::SetActivePinDepth(
	int32_t	inCustomDepth,
	bool	inUpdate)
{
	if (mActivePin != eNoSubField)
	{
		if (inCustomDepth)
		{
			uint8_t	pinIndex = mSpec.Dec22mmToIndex(inCustomDepth);
			/*
			*	If inCustomDepth isn't custom THEN
			*	clear the custom value and update the pin index
			*/
			if (pinIndex != SKeySpec::eInvalidIndex)
			{
				inCustomDepth = 0;
				mPins[mActivePin] = pinIndex;
				mCustomCutDepths[mActivePin] = 1;	// To force an update if not already custom
			}
		}
		/*
		*	If inCustomDepth is non-zero THEN
		*	denote this pin as custom (if not already)
		*/
		if (inCustomDepth)
		{
			if (mCustomCutDepths[mActivePin] == 0)
			{
				mSubFieldStrs[mActivePin] = kCustomPinDepthStr;
				mDirtyPin |= _BV(mActivePin);
			}
		/*
		*	Else, if the active pin is custom THEN
		*	clear the custom value (if custom)
		*/
		} else if (mCustomCutDepths[mActivePin] != 0)
		{
			mSubFieldStrs[mActivePin] = kDecStrs[mPins[mActivePin]];;
			mDirtyPin |= _BV(mActivePin);
		}
		mCustomCutDepths[mActivePin] = inCustomDepth;
		if (inUpdate && mDirtyPin)
		{
			DrawFields();
		}
	}
}

/**************************** UpdateStringForValue ****************************/
void KMPinsValueField::UpdateStringForValue(void)
{
	mDirtyPin = 0;
	uint8_t	pinIndex;
	int32_t	pinValue = mValue;
	for (int32_t i = mPinCount-1; i >= 0; i--)
	{
		pinIndex = (pinValue % 10);
		pinValue /= 10;
		if (mPins[i] != pinIndex)
		{
			mPins[i] = pinIndex;
			mDirtyPin |= _BV(i);
			if (mCustomCutDepths[i] == 0)
			{
				mSubFieldStrs[i] = kDecStrs[pinIndex];
			} else
			{
				mSubFieldStrs[i] = kCustomPinDepthStr;
			}
		}
	}
}
