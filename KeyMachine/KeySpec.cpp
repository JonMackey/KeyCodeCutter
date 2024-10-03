/*
*	KeySpec.cpp, Copyright Jonathan Mackey 2024
*
*	This is a minimal parser for key=value files of the same form as
*	boards.txt and platform.txt accept this class interprets the values as
*	either strings or numbers with support for notted values and hex values.
*	Only the select set of keys defined below are read.  Any other keys are
*	skipped.
*
*	To keep the code size small, very little error checking performed.
*	It's assumed that the config file was created by KeySpec::WriteFile
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
#include "KeySpec.h"
#ifndef __MACH__
#include <Arduino.h>
#include "SdFat.h"
#include "sdios.h"
#include "Config.h"
#else
#include <string>
#define _BV(bit) (1 << (bit))
namespace Config
{
	const uint32_t	kKeyHolderDepth = 457;	// = 4.57mm = 0.180 inches
}
#endif
//#include "UnixTime.h"

/*
*	List of keys
*/
const char kNameKeyStr[] = "name";
const char kCutAngleKeyStr[] = "cutAngle";
const char kPinDepthIncKeyStr[] = "pinDepthInc";
const char kDeepestCutKeyStr[] = "deepestCut";
const char kBladeWidthKeyStr[] = "bladeWidth";
const char kFlatWidthKeyStr[] = "flatWidth";
const char kPinSpacingKeyStr[] = "pinSpacing";
const char kFirstPinCenterKeyStr[] = "firstPinCenter";
const char kMACSKeyStr[] = "macs";
const char kShalowestCutIndexKeyStr[] = "shalowestCutIndex";
const char kDeepestCutIndexKeyStr[] = "deepestCutIndex";
const char kIsMetricKeyStr[] = "isMetric";
const char kIncreasingDepthsKeyStr[] = "increasingDepths";
const char kTagKeyStr[] = "tag";
const char kPinsSupportedKeyStr[] = "pinsSupported";

const char* const kSpecKeys[] =
{	// Sorted alphabetically by key string
	kBladeWidthKeyStr, 		
	kCutAngleKeyStr,		
	kDeepestCutKeyStr, 		
	kDeepestCutIndexKeyStr,
	kFirstPinCenterKeyStr, 
	kFlatWidthKeyStr, 
	kIncreasingDepthsKeyStr,		
	kIsMetricKeyStr, 		
	kMACSKeyStr, 			
	kNameKeyStr, 			
	kPinDepthIncKeyStr, 	
	kPinSpacingKeyStr, 		
	kPinsSupportedKeyStr, 		
	kShalowestCutIndexKeyStr ,
	kTagKeyStr
};

enum EKeyIndexes
{
	eInvalidKeyIndex,
	// Must align with eSettingsKeys
	eBladeWidth, 		
	eCutAngle,		
	eDeepestCut, 		
	eDeepestCutIndex,
	eFirstPinCenter, 
	eFlatWidth,
	eIncreasingDepths,	// bool
	eIsMetric, 		
	eMACS, 			
	eName, 			
	ePinDepthInc, 	
	ePinSpacing, 		
	ePinsSupported, 		
	eShalowestCutIndex,
	eTag,				// uint16
	eKeyCount
};

static const uint32_t kUInt32KeysMask =
						_BV(eDeepestCutIndex) |
						_BV(eMACS) |
						_BV(ePinsSupported) |
						_BV(eShalowestCutIndex) |
						_BV(eTag);
static const uint32_t kFloatKeysMask =
						_BV(eBladeWidth) |
						_BV(eCutAngle) |
						_BV(eDeepestCut) |
						_BV(eFirstPinCenter) |
						_BV(eFlatWidth) |
						_BV(ePinDepthInc) |
						_BV(ePinSpacing);
static const uint32_t kBoolKeysMask =
						_BV(eIncreasingDepths) |
						_BV(eIsMetric);

/********************************* KeySpec **********************************/
KeySpec::KeySpec(void)
: mFile(nullptr)
{
}

/********************************** ReadFile **********************************/
/*
*	It's assumed SdFat.begin was successfully called prior to calling this
*	routine.
*
*	This routine returns true if the file contains all of the key values.
*/
bool KeySpec::ReadFile(
	const char*	inPath)
{
	mSpec = {0};
	uint8_t	requiredKeyValues = 0;
#ifndef __MACH__
	SdFile file;
	bool	fileOpened = file.open(inPath, O_RDONLY);
	mFile = &file;
#else
	mFile = fopen(inPath, "r+");
	bool	fileOpened = mFile != nullptr;
#endif
	if (fileOpened)
	{
		char	thisChar = NextChar();
		while (thisChar)
		{
			thisChar = SkipWhitespaceAndHashComments(thisChar);
			if (thisChar)
			{
				char keyStr[32];
				keyStr[0] = thisChar;
				thisChar = ReadStr('=', 31, &keyStr[1]);
				while (thisChar)
				{
					StripLeadingAndTrailingWhitespace(keyStr);
					uint8_t keyIndex = FindKeyIndex(keyStr);
					if (keyIndex)
					{
						requiredKeyValues++;
						if (kUInt32KeysMask & (_BV(keyIndex)))
						{
							uint32_t	value;
							thisChar = ReadUInt32Number(value);
							switch (keyIndex)
							{
								case eDeepestCutIndex:
									mSpec.deepestCutIndex = value;
									break;
								case eMACS:
									mSpec.macs = value;
									break;
								case ePinsSupported:
									mSpec.pinsSupported = value;
									break;
								case eShalowestCutIndex:
									mSpec.shalowestCutIndex = value;
									break;
							}
						} else if (kFloatKeysMask & (_BV(keyIndex)))
						{
							float	fValue;
							thisChar = ReadFloatNumber(fValue);
							switch (keyIndex)
							{
								case eBladeWidth:
									mSpec.bladeWidth = fValue;
									break;
								case eCutAngle:
									mSpec.cutAngle = fValue;
									break;
								case eDeepestCut:
									mSpec.deepestCut = fValue;
									break;
								case eFirstPinCenter:
									mSpec.firstPinCenter = fValue;
									break;
								case eFlatWidth:
									mSpec.flatWidth = fValue;
									break;
								case ePinDepthInc:
									mSpec.pinDepthInc = fValue;
									break;
								case ePinSpacing:
									mSpec.pinSpacing = fValue;
									break;
							}
						} else if (kBoolKeysMask & (_BV(keyIndex)))
						{
							char valueStr[10];
							thisChar = ReadStr('\n', sizeof(valueStr), valueStr);
							StripLeadingAndTrailingWhitespace(valueStr);
							bool isTrue =  valueStr[0] == 't';
							switch (keyIndex)
							{
								case eIncreasingDepths:
									mSpec.increasingDepths = isTrue;
									break;
								case eIsMetric:
									mSpec.isMetric = isTrue;
									break;
							}
						} else if (keyIndex == eName)
						{
							thisChar = ReadStr('\n', sizeof(mSpec.name), mSpec.name);
							StripLeadingAndTrailingWhitespace(mSpec.name);
						}
					} else
					{
						thisChar = SkipToNextLine();
					}
					break;
				}
			}
		}
	#ifndef __MACH__
		mFile->close();
	#else
		fclose(mFile);
	#endif
		mFile = nullptr;
	}
	return(requiredKeyValues == (eKeyCount-1));
}

/********************************** NextChar **********************************/
char KeySpec::NextChar(void)
{
	char	thisChar;
#ifdef __MACH__
	thisChar = getc(mFile);
	if (thisChar == -1)
	{
		thisChar = 0;
	}
#else
	if (mFile->read(&thisChar,1) != 1)
	{
		thisChar = 0;
	}
#endif
	return(thisChar);
}

/******************************** FindKeyIndex ********************************/
/*
*	Returns the index of inKey within the array kSpecKeys + 1.
*	Returns 0 if inKey is not found.
*/
uint8_t KeySpec::FindKeyIndex(
	const char*	inKey)
{
	uint8_t leftIndex = 0;
	uint8_t rightIndex = (sizeof(kSpecKeys)/sizeof(char*)) -1;
	while (leftIndex <= rightIndex)
	{
		uint8_t current = (leftIndex + rightIndex) / 2;
		const char* currentPtr = kSpecKeys[current];
		int cmpResult = strcmp(inKey, currentPtr);
		if (cmpResult == 0)
		{
			return(current+1);	// Add 1, 0 is reserved for "not found"
		} else if (cmpResult <= 0)
		{
			rightIndex = current - 1;
		} else
		{
			leftIndex = current + 1;
		}
	}
	return(0);
}

/********************** StripLeadingAndTrailingWhitespace *********************/
void KeySpec::StripLeadingAndTrailingWhitespace(
	char*	ioStr)
{
	char*	strPtr = ioStr;
	char	thisChar;
	/*
	*	Strip any leading whitespace...
	*/
	for (thisChar = *(strPtr++); thisChar != 0; thisChar = *(strPtr++))
	{
		if (isspace(thisChar))
		{
			continue;
		}
		*(ioStr++) = thisChar;
		break;
	}
	/*
	*	Skip any trailing whitespace...
	*/
	char*	endWhitespace = nullptr;
	for (thisChar = *(strPtr++); thisChar != 0; thisChar = *(strPtr++))
	{
		if (!isspace(thisChar))
		{
			endWhitespace = nullptr;
		} else if (endWhitespace == nullptr)
		{
			endWhitespace = ioStr;
		}
		*(ioStr++) = thisChar;
	}
	if (endWhitespace == nullptr)
	{
		*ioStr = 0;
	} else
	{
		*endWhitespace = 0;
	}
}

/*********************** SkipWhitespaceAndHashComments ************************/
char KeySpec::SkipWhitespaceAndHashComments(
	char	inCurrChar)
{
	char	thisChar = inCurrChar;
	while (thisChar)
	{
		if (isspace(thisChar))
		{
			thisChar = NextChar();
			continue;
		} else if (thisChar == '#')
		{
			thisChar = SkipToNextLine();
			continue;
		}
		break;
	}
	return(thisChar);
}

/******************************* SkipToNextLine *******************************/
/*
*	Returns the character following the newline (if any).
*	Does not support Windows CRLF line endings.
*/
char KeySpec::SkipToNextLine(void)
{
	char thisChar = NextChar();
	for (; thisChar; thisChar = NextChar())
	{
		if (thisChar != '\n')
		{
			continue;
		}
		thisChar = NextChar();
		break;
	}
	return(thisChar);
}

/****************************** ReadUInt32Number ******************************/
char KeySpec::ReadUInt32Number(
	uint32_t&	outValue)
{
	bool		bitwiseNot = false;
	bool 		isHex = false;
	uint32_t	value = 0;
	char		thisChar = SkipWhitespaceAndHashComments(NextChar());
	if (thisChar)
	{
		bitwiseNot = thisChar == '~';
		/*
		*	If notted THEN
		*	get the next char after the not.
		*/
		if (bitwiseNot)
		{
			thisChar = SkipWhitespaceAndHashComments(NextChar());
		}
		if (thisChar == '0')
		{
			thisChar = NextChar();	// Get the character following the leading zero.
			isHex = thisChar == 'x';
			/*
			*	If this is a hex prefix THEN
			*	get the character following it.
			*/
			if (isHex)
			{
				thisChar = NextChar();
			}
		}
		// Now just consume characters till a non valid numeric char is hit.
		if (isHex)
		{
			for (; isxdigit(thisChar); thisChar = NextChar())
			{
				thisChar -= '0';
				if (thisChar > 9)
				{
					thisChar -= 7;
					if (thisChar > 15)
					{
						thisChar -= 32;
					}
				}
				value = (value << 4) + thisChar;
			}
		} else
		{
			for (; isdigit(thisChar); thisChar = NextChar())
			{
				value = (value * 10) + (thisChar - '0');
			}
		}
		thisChar = SkipWhitespaceAndHashComments(thisChar);
	}
	if (bitwiseNot)
	{
		value = ~value;
	}
	outValue = value;
	return(thisChar);
}

/****************************** ReadFloatNumber *******************************/
char KeySpec::ReadFloatNumber(
	float&	outValue)
{
	char	numBuf[20];
	char	thisChar = ReadStr('\n', sizeof(numBuf), numBuf);
	sscanf(numBuf, "%f", &outValue);

	return(thisChar);
}

/********************************** ReadStr ***********************************/
/*
*	Reads the string till the delimiter is hit.
*	Once the string is full (inMaxStrLen), characters are ignored till the
*	delimiter is hit.
*	Hash (#) comments are not allowed because the strings aren't quoted.
*/
char KeySpec::ReadStr(
	char	inDelimiter,
	uint8_t	inMaxStrLen,
	char*	outStr)
{
	char		thisChar;
	char*		endOfStrPtr = &outStr[inMaxStrLen -1];

	while((thisChar = NextChar()) != 0)
	{
		if (thisChar != inDelimiter)
		{
			/*
			*	If outStr isn't full THEN
			*	append thisChar to it.
			*/
			if (outStr < endOfStrPtr)
			{
				*(outStr++) = thisChar;
			} // else discard the character, the outStr is full
			continue;
		}
		break;
	}
	*outStr = 0;	// Terminate the string
	return(thisChar);
}

/******************************** DepthAtIndex ********************************/
/*
*	Returns the cut depth, the distance from the bottom of the key blade to the
*	cut flat.
*	Returns zero if inIndex in invalid in terms of index only, not in terms of
*	MACS.
*/
float SKeySpec::DepthAtIndex(
	uint32_t	inIndex) const
{
	float depth = 0;
	if (increasingDepths)
	{
		if (inIndex >= shalowestCutIndex &&
			inIndex <= deepestCutIndex)
		{
			depth = ((deepestCutIndex - inIndex)*pinDepthInc) + deepestCut;
		}
	} else if (inIndex >= deepestCutIndex &&
			inIndex <= shalowestCutIndex)
	{
		depth = (inIndex*pinDepthInc) + deepestCut;
	}
	return(depth);
}

/******************************* FloatToDec22mm *******************************/
/*
*	Converts a float to a int32_t where the last 2 digits are 100th of a mm.
*	e.g. 1.05mm is converted to 105.
*	For imperial/inch values, the value is converted to mm.
*	Dec22 works nicely for both UI and stepper values.  The stepper moves at
*	1/100 per step.
*/
int32_t SKeySpec::FloatToDec22mm(
	float	inValue) const
{
	return(isMetric ? inValue*100 : inValue*2540);
}

/******************************* Dec22mmToFloat *******************************/
float SKeySpec::Dec22mmToFloat(
	int32_t	inDec22) const
{
	return(isMetric ? ((float)inDec22)/100 : std::round(((float)inDec22)/2.54)/1000);
}

/******************************* IndexToDec22mm *******************************/
int32_t SKeySpec::IndexToDec22mm(
	uint32_t	inIndex) const
{
	return(FloatToDec22mm(DepthAtIndex(inIndex)));
}

/******************************* Dec22mmToIndex *******************************/
uint32_t SKeySpec::Dec22mmToIndex(
	int32_t	inDec22) const
{
	/*
	*	Because the dec22 mm values are stored as converted values from float,
	*	to avoid rounding errors when the spec is in inches, loop through all of
	*	the possible calculated values rather than use simple division.
	*/
	uint32_t	i, lastIndex;
	if (increasingDepths)
	{
		i = shalowestCutIndex;
		lastIndex = deepestCutIndex;
	} else
	{
		i = deepestCutIndex;
		lastIndex = shalowestCutIndex;
	}
	for (; i <= lastIndex; i++)
	{
		if (inDec22 != IndexToDec22mm(i))
		{
			continue;
		}
		return(i);
	}
	return(eInvalidIndex);
}

/****************************** PinCodeToDec22mm ******************************/
/*
*	This routine was written to support KMPinsValueField::GetPinDepthsDec22mm().
*	Converts inPinCode, e.g. 35627, to an array of Dec22 mm depth values.
*	inOverrideArray is an int32 array of length inPinCount. Non-zero values are
*	cut depth overrides (Dec22 mm.)
*	outPinArray is an int32 array of length inPinCount.
*	outErrorPin is an optional ptr to storage for the pin related to the error.
*/
SKeySpec::EErrorCode SKeySpec::PinCodeToDec22mm(
	uint32_t		inPinCode,
	uint32_t		inPinCount,
	const int32_t	inOverrideArray[],
	int32_t			outPinArray[],
	uint32_t*		outErrorPin) const
{
	EErrorCode err = eNoErr;
	int32_t	cutDepth;
	int32_t	prevCutDepth = 0;
	int32_t	dec22MACS = FloatToDec22mm(macs*pinDepthInc);
	
	// Convert to base 10 to extract the root pin depth indexes.
	for (int32_t i = inPinCount-1; i >= 0; i--)
	{
		int32_t	rootIndex = (inPinCode % 10);
		/*
		*	If there's no depth override for this pin THEN
		*	calculate and verify the cut depth.
		*/
		if (inOverrideArray[i] == 0)
		{
			cutDepth = IndexToDec22mm(rootIndex);
			if (cutDepth == 0)
			{
				if (outErrorPin)
				{
					*outErrorPin = i;
				}
				err = ePinIndexErr;
				break;
			}
		/*
		*	Else, use the override
		*/
		} else
		{
			cutDepth = inOverrideArray[i];
		}
		/*
		*	If the cutDepth isn't going to cause the cutter to damage the key holder...
		*/
		if (cutDepth > Config::kKeyHolderDepth)
		{
			/*
			*	If this isn't the first cut THEN
			*	validate the cut for MACS.
			*/
			if (prevCutDepth)
			{
				int32_t pinDelta = cutDepth - prevCutDepth;
				if (pinDelta < 0)
				{
					pinDelta = -pinDelta;
				}
				if (pinDelta > dec22MACS)
				{
					err = eMACSErr;
					break;
				}
			}
			outPinArray[i] = prevCutDepth = cutDepth;
			inPinCode /= 10;
		} else
		{
			if (outErrorPin)
			{
				*outErrorPin = i;
			}
			err = eDepthMaxErr;
			break;
		}
	}
	return(err);
}

/****************************** PinCountSupported *****************************/
bool SKeySpec::PinCountSupported(
	uint32_t	inPinCount) const
{
	bool isValid = false;
	if (inPinCount <= eMaxPinCount &&
		inPinCount >= eMinPinCount)
	{
		uint16_t	pinMask = _BV(inPinCount);
		isValid = (pinsSupported & pinMask) != 0;
	}
	return(isValid);
}

#if 0
/*************************** FirstSupportedPinCount ***************************/
uint32_t SKeySpec::FirstSupportedPinCount(void) const
{
	uint16_t	pinMask = _BV(eMinPinCount);
	uint32_t	pinCount = eMinPinCount;
	for (; pinCount < eMaxPinCount; pinMask <<= 1, pinCount++)
	{
		if ((pinsSupported & pinMask) == 0)
		{
			continue;
		}
		break;
	}
	return(pinCount);
}

/********************************** Validate **********************************/
/*
*	inPinArray is an array of key cut depth Dec22 mm values.
*/
SKeySpec::EErrorCode SKeySpec::Validate(
	uint32_t		inPinCount,
	const int32_t	inPinArray[]) const
{
	EErrorCode err = eNoErr;
	int32_t	dec22MACS = FloatToDec22mm(macs*pinDepthInc);
	
	for (int32_t i = 0; i < inPinCount; i++)
	{
		int32_t cutDepth = inPinArray[i];
		/*
		*	If the cutDepth isn't going to cause the cutter to damage the key holder...
		*/
		if (cutDepth > Config::kKeyHolderDepth)
		{
			/*
			*	If this isn't the first cut THEN
			*	validate the cut for MACS.
			*/
			if (i)
			{
				int32_t pinDelta = cutDepth - inPinArray[i-1];
				if (pinDelta < 0)
				{
					pinDelta = -pinDelta;
				}
				if (pinDelta > dec22MACS)
				{
					err = eMACSErr;
					break;
				}
			}
		} else
		{
			err = eDepthMaxErr;
			break;
		}
	}
	return(err);
}
#endif
