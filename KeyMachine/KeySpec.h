/*
*	KeySpec.h, Copyright Jonathan Mackey 2024
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


#ifndef KeySpec_h
#define KeySpec_h

#include <inttypes.h>

#ifdef __MACH__
#include <stdio.h>
#define SdFile	FILE
#else
class SdFile;
#endif

struct SKeySpec
{
	char		name[20];
	float		cutAngle;		// Tangent relative to 0
	float		pinDepthInc;
	float		deepestCut;
	float		bladeWidth;
	float		flatWidth;
	float		pinSpacing;		// Pin center to center
	float		firstPinCenter;
	/*
	*	MACS (Maximum Adjacent Cut Specification): the maximum allowable
	*	difference between cut depths.  This is provided by the manufacturer
	*	as an integer.
	*/
	uint32_t	macs;
	uint32_t	shalowestCutIndex;
	uint32_t	deepestCutIndex;
	bool		isMetric;
	bool		increasingDepths;
	uint16_t	tag;	// Used by UI to quickly identify this key spec.
	/*
	*	pinsSupported is a bit field - allowed pin counts for this spec.
	*			pins  7654 3210
	*	e.g. 0x60 = 0b0110 0000 -> supports 6 & 5 pin
	*	e.g. 0x50 = 0b0101 0000 -> supports 6 & 4 pin
	*/
	enum EPinsSupported
	{
		eMinPinCount	= 4,
		eMaxPinCount	= 6,
		e4PinMask		= 0x10,
		e5PinMask		= 0x20,
		e6PinMask		= 0x40
	};
	uint16_t	pinsSupported;
	enum EIndex
	{
		eInvalidIndex	= 99
	};
	enum EErrorCode
	{
		eNoErr,
		eMACSErr,
		ePinIndexErr,	// Pin index out of range
		eDepthMaxErr	// Out of range kKeyHolderDepth to inBladeWidth
	};
	
	float					DepthAtIndex(
								uint32_t				inIndex) const;
	int32_t					FloatToDec22mm(
								float					inValue) const;
	float					Dec22mmToFloat(
								int32_t					inDec22) const;
	int32_t					IndexToDec22mm(
								uint32_t				inIndex) const;
	uint32_t				Dec22mmToIndex(
								int32_t					inDec22) const;
	SKeySpec::EErrorCode	PinCodeToDec22mm(
								uint32_t				inPinCode,
								uint32_t				inPinCount,
								const int32_t			inOverrideArray[],
								int32_t					outPinArray[],
								uint32_t*				outErrorPin = nullptr) const;
	bool					PinCountSupported(
								uint32_t				inPinCount) const;
//	uint32_t				FirstSupportedPinCount(void) const;
//	SKeySpec::EErrorCode	Validate(
//								uint32_t				inPinCount,
//								const int32_t			inPinArray[]) const;
};

class KeySpec
{
public:
							KeySpec(void);
	bool					ReadFile(
								const char*				inPath);
	const SKeySpec&			Spec(void) const
								{return(mSpec);}
protected:
	SdFile*		mFile;
	SKeySpec	mSpec;
	
	char					NextChar(void);
	uint8_t					FindKeyIndex(
								const char*				inKey);
	void					StripLeadingAndTrailingWhitespace(
								char*					ioStr);
	char					SkipWhitespaceAndHashComments(
								char					inCurrChar);
	char					SkipToNextLine(void);
	char					ReadUInt32Number(
								uint32_t&				outValue);
	char					ReadFloatNumber(
								float&					outValue);
	char					ReadStr(
								char					inDelimiter,
								uint8_t					inMaxStrLen,
								char*					outStr);
};

#endif /* KeySpec_h */
