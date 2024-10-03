/*
*	CutKey.cpp, Copyright Jonathan Mackey 2024
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
#include "CutKey.h"
#ifndef __MACH__
#include "Config.h"
#else
#include <string>
#define _BV(bit) (1 << (bit))
namespace Config
{
	const uint32_t	kKeyHolderDepth = 457;	// = 4.57mm = 0.180 inches
}
#endif

const char	CutKey::kName[] = "Cut Key";

#ifndef __MACH__
/*********************************** CutKey ***********************************/
CutKey::CutKey(
	StepControl*	inController,
	Stepper*		inXStepper,
	Stepper*		inZStepper)
	: mController(inController), mXStepper(inXStepper), mZStepper(inZStepper)
{
}
#endif

/************************************ Setup ***********************************/
/*
*	inSpec is the SKeySpec of key to be cut.
*	inOriginX is the number of steps from the X max Endstop to the key shoulder.
*	inOriginZ is the number of steps from the Y max Endstop to the bottom of the
*	key hplder.
*	inPinDepthArray are the cut depths in dec22 mm format.  These values get
*	converted to float values in the unit defined in inSpec.  inPinDepthArray
*	is optional.  If it's not set here then it's assumed a version of
*	LoadCutDepths() will be used.
*/
void CutKey::Setup(
	const SKeySpec*	inSpec,
	uint32_t		inPinCount,
	int32_t			inOriginX,
	int32_t			inOriginZ,
	int32_t*		inPinDepthArray) // Length = inPinCount
{
	mSpec = *inSpec;
	mFlatSpacing = inSpec->pinSpacing - inSpec->flatWidth;
	m1stFlatLeft = inSpec->firstPinCenter - (inSpec->flatWidth/2);
	mCutAngleIntersectionX = mFlatSpacing/2;
	mCutAngleIntersectionZ = mCutAngleIntersectionX*inSpec->cutAngle;
	mMoveIndex = 0;
	mCutDepths[0] = 0;
	mOriginX = inOriginX;
	mOriginZ = inOriginZ;
	//Serial.printf("mOriginX = %d, mOriginZ = %d\n", mOriginX, mOriginZ);
	LoadDec22mmCutDepths(inPinCount, inPinDepthArray);
}

/************************************ begin ***********************************/
/*
*	begin() should be called after Setup().
*
*	The position of the cutter wheel when begin() is called is 10mm above, and
*	0.5mm to the the right of the key origin (i.e. bottom of the key holder, key
*	shoulder.)
*
*	The function NextMove returns values relative to the key origin.
*
*	On both axis, movement towards a max endstop is negative
*	
*					|<------------------------->| = inOriginX
*												0 = X home
*		Key Shoulder|<-->|<-------------------->|X max Endstop
*					|<-->| = Initial offset off origin is 0.5mm or 50 steps
*							= inOriginX - 50
*		
*		
*					|<------------------------->| = inOriginZ
*												0 = Z home
*	Bottom of Holder|<-->|<-------------------->|Z max Endstop
*					|<-->| = Initial offset off origin is 10mm or 1000 steps
*							= inOriginX - 1000
*/
void CutKey::begin(void)
{
	mExitState = eActionFailed;
	mMoveIndex = 0;
#ifndef __MACH__
	mXStepper->setMaxSpeed(TO_MICROSTEPS(50));			// steps/s
	mXStepper->setAcceleration(TO_MICROSTEPS(25));			// steps/s^2 
	mZStepper->setMaxSpeed(TO_MICROSTEPS(50));			// steps/s
	mZStepper->setAcceleration(TO_MICROSTEPS(25));			// steps/s^2 
#endif
}

/*********************************** IsDone ***********************************/
bool CutKey::IsDone(void)
{
	bool	done = false;
#ifndef __MACH__
	if (!mController->isRunning())
	{
		float	X, Z;
		
		if (NextMove(X, Z))
		{
			int32_t	xPos = mOriginX + mSpec.FloatToDec22mm(X);
			int32_t	zPos = mOriginZ - mSpec.FloatToDec22mm(Z);
			/*Serial.printf("X,Z = %d,%d, xPos = %d, zPos = %d = %d\n",
				mSpec.FloatToDec22mm(X), mSpec.FloatToDec22mm(Z),
				xPos, zPos, mSpec.Dec22mmToIndex(mSpec.FloatToDec22mm(Z)));	*/	
#if 1
			mXStepper->setTargetAbs(TO_MICROSTEPS(xPos));
			
			if (zPos != TO_STEPS(mZStepper->getPosition()))
			{
				mZStepper->setTargetAbs(TO_MICROSTEPS(zPos));
				mController->moveAsync(*mXStepper, *mZStepper);
			} else
			{
				mController->moveAsync(*mXStepper);
			}
#endif
		} else
		{
			mExitState = eExitNormal;
			done = true;
		}
	}
#endif
	return(done);
}

/**************************** LoadDec22mmCutDepths ****************************/
/*
*	This should be called after mSpec has been initialized.
*
*	inPinDepthArray are the cut depths in dec22 mm format.  These values get
*	converted to float values in the unit defined in mSpec.
*	
*	Unlike LoadCutDepths(), inPinDepthArray values must already have been
*	validated.
*/
void CutKey::LoadDec22mmCutDepths(
	uint32_t	inPinCount,
	int32_t*	inPinDepthArray) // Length = inPinCount
{
	/*
	*	If there's a pin depth array THEN
	*	convert it to floats in the unit defined in mSpec.
	*/
	if (inPinDepthArray)
	{
		mPinCount = inPinCount;
		for (int32_t i = 0; i < inPinCount; i++)
		{
			mCutDepths[i] = mSpec.Dec22mmToFloat(inPinDepthArray[i]);
		}
		mCutDepths[inPinCount] = mSpec.bladeWidth;
	} else
	{
		mPinCount = 0;
		mCutDepths[0] = 0;
	}
}

#if __MACH__
/******************************** LoadCutDepths *******************************/
/*
*	This should be called after mSpec has been initialized.
*
*	inPinCode: e.g. 35627
*/
SKeySpec::EErrorCode CutKey::LoadCutDepths(
	uint32_t	inPinCount,
	uint32_t	inPinCode)
{
	SKeySpec::EErrorCode err = SKeySpec::eNoErr;
	int32_t	prevRootIndex = -1;
	float	cutDepth;
	float	keyHolderDepth = (float)Config::kKeyHolderDepth/100;
	mPinCount = mErrorPin = inPinCount;
	if (!mSpec.isMetric)
	{
		keyHolderDepth/=25.4;
	}
	// Convert to base 10 to extract the root pin depth indexes.
	for (int32_t i = inPinCount-1; i >= 0; i--)
	{
		int32_t	rootIndex = (inPinCode % 10);
		cutDepth = mSpec.DepthAtIndex(rootIndex);
		if (cutDepth == 0)
		{
			mErrorPin = i;
			err = SKeySpec::ePinIndexErr;
			break;
		}
		/*
		*	If the cutDepth isn't going to cause the cutter to damage the key holder...
		*/
		if (cutDepth > keyHolderDepth)
		{
			/*
			*	If this isn't the first cut THEN
			*	validate the cut for MACS.
			*/
			if (prevRootIndex >= 0)
			{
				int32_t pinDelta = rootIndex - prevRootIndex;
				if (pinDelta < 0)
				{
					pinDelta = -pinDelta;
				}
				if (pinDelta > mSpec.macs)
				{
					err = SKeySpec::eMACSErr;
					break;
				}
			}
			mCutDepths[i] = cutDepth;
			prevRootIndex = rootIndex;
			inPinCode /= 10;
		} else
		{
			mErrorPin = i;
			err = SKeySpec::eDepthMaxErr;
			break;
		}
	}
	if (!err)
	{
		mCutDepths[inPinCount] = mSpec.bladeWidth;
	} else
	{
		mCutDepths[0] = 0;
	}
	return(err);
}

/******************************** LoadCutDepths *******************************/
/*
*	This should be called after mSpec has been initialized.
*
*	Load non-code cut depths (Master keys, etc..)
*/
SKeySpec::EErrorCode CutKey::LoadCutDepths(
	uint32_t	inPinCount,
	const float	inPinDepthArray[])
{
	SKeySpec::EErrorCode err = SKeySpec::eNoErr;
	float	fMACS = (mSpec.macs * mSpec.pinDepthInc) + 0.0005;
	float	keyHolderDepth = (float)Config::kKeyHolderDepth/100;
	mPinCount = inPinCount;
	if (!mSpec.isMetric)
	{
		keyHolderDepth/=25.4;
	}
	
	for (int32_t i = 0; i < inPinCount; i++)
	{
		float cutDepth = inPinDepthArray[i];
		/*
		*	If the cutDepth isn't going to cause the cutter to damage the key holder...
		*/
		if (cutDepth > keyHolderDepth)
		{
			/*
			*	If this isn't the first cut THEN
			*	validate the cut for MACS.
			*/
			if (i)
			{
				float pinDelta = cutDepth - mCutDepths[i-1];
				if (pinDelta < 0)
				{
					pinDelta = -pinDelta;
				}
				if (pinDelta > fMACS)
				{
					err = SKeySpec::eMACSErr;
					break;
				}
			}
			mCutDepths[i] = cutDepth;
		} else
		{
			err = SKeySpec::eDepthMaxErr;
			break;
		}
	}
	if (!err)
	{
		mCutDepths[inPinCount] = mSpec.bladeWidth;
	} else
	{
		mCutDepths[0] = 0;
	}
	return(err);
}
#endif

/********************************** NextMove **********************************/
/*
*	The first two bits of mMoveIndex define 4 possible moves (X,Z):
*	0 - special case, first move to leftmost position on top edge of key blank.
*	1 - move to the left side of the key flat.
*	2 - move the right side of the key flat. (X move only)
*	3 - move to the diagonal intersection of the next pin's key flat.
*
*	Note that not all pins require all three moves.  Pins whose diagonal
*	intersection is the key flat will not have move 1 because the cutter is
*	already on the start of the key flat.
*
*	The remaining bits of mMoveIndex is the pin index (mMoveIndex >> 2)
*/
bool CutKey::NextMove(
	float&	outX,
	float&	outZ)
{
	uint32_t	pinIndex = mMoveIndex >> 2;
	bool	canMove = pinIndex < mPinCount;
	if (canMove)
	{
		switch (mMoveIndex & 3)
		{
			case 0:
				/*
				*	Special case, first move
				*	Move to the leftmost position on the top edge of the key blank.
				*/
				outX = m1stFlatLeft - ((mSpec.bladeWidth - mCutDepths[0])/mSpec.cutAngle);
				outZ = mSpec.bladeWidth;
				break;
			case 1:
				/*
				*	Move the the left side of the current flat
				*/
				outX = m1stFlatLeft + (pinIndex * mSpec.pinSpacing);
				outZ = mCutDepths[pinIndex];
				break;
			case 2:
			{
				/*
				*	Move to the right side of the current flat
				*/
				outX = m1stFlatLeft + mSpec.flatWidth + (pinIndex * mSpec.pinSpacing);
				outZ = mLastZ;
				float nextX, nextZ;
				NextIntersection(nextX, nextZ);
				/*
				*	If this flat right is to the right of the next intersection THEN
				*	shorten the flat length so that the next diagonal starts at the flat.
				*/
				if (outX > nextX)
				{
					outX = nextX - (outZ - nextZ)/mSpec.cutAngle;
					mMoveIndex += 2;	// Skip case 3 and special case 0
				}
				break;
			}
			case 3:
				/*
				*	Move to the intersection of the right diagonal and the
				*	left diagonal of the next pin's flat.
				*
				*	If the next flat left is to the left of the intersection THEN
				*	extend the left diagonal past the intersection to the next flat.
				*/
				if (NextIntersection(outX, outZ))
				{
					float nextZ = mCutDepths[pinIndex+1];
					outX += (nextZ - outZ)/mSpec.cutAngle;
					outZ = nextZ;
					mMoveIndex += 2;	// Skip special case 0 and case 1
				} else
				{
					mMoveIndex++;	// Skip special case 0
				}
				break;
		}
		mMoveIndex++;
		mLastX = outX;
		mLastZ = outZ;
	}
	return(canMove);
}

/****************************** NextIntersection ******************************/
bool CutKey::NextIntersection(
	float&	outX,
	float&	outZ)
{
	uint32_t	pinIndex = mMoveIndex >> 2;
	// Note that mCutDepths[pinIndex+1] works for the last pin
	// because there's a dummy depth at the end of the array.
	float nextCutDepth = mCutDepths[pinIndex+1];
	float halfZDelta = (nextCutDepth - mLastZ)/2;
	float xDelta = halfZDelta/mSpec.cutAngle;
	float nextFlatLeft = m1stFlatLeft + ((pinIndex+1) * mSpec.pinSpacing);
	outX = mCutAngleIntersectionX + xDelta + m1stFlatLeft + mSpec.flatWidth + (pinIndex * mSpec.pinSpacing);
	outZ = mLastZ + mCutAngleIntersectionZ + halfZDelta;
	return(nextFlatLeft < outX);
}


