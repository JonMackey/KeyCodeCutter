/*
*	CutKey.h, Copyright Jonathan Mackey 2024
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


#ifndef CutKey_h
#define CutKey_h

#include "KMAction.h"
#include "KeySpec.h"

class CutKey : public KMAction
{
public:
#ifndef __MACH__
							CutKey(
								StepControl*			inController,
								Stepper*				inXStepper,
								Stepper*				inZStepper);
#else
							CutKey(void){}
#endif
	virtual void			begin(void);
	virtual bool			IsDone(void);
	virtual const char*		Name(void) const
								{return(kName);}
	void					Setup(
								const SKeySpec*			inSpec,
								uint32_t				inPinCount,
								int32_t					inOriginX,
								int32_t					inOriginZ,
								int32_t*				inPinDepthArray = nullptr);

	void					LoadDec22mmCutDepths(
								uint32_t				inPinCount,
								int32_t*				inPinDepthArray);
#if __MACH__
	SKeySpec::EErrorCode	LoadCutDepths(
								uint32_t				inPinCount,
								uint32_t				inPinCode);		// Bow to tip
							/*
							*	Custom loading of pin depths
							*	inPinArray length = mPinCount
							*/
	SKeySpec::EErrorCode	LoadCutDepths(
								uint32_t				inPinCount,
								const float				inPinDepthArray[]);	// Bow to tip
#endif
							/*
							*	When LoadCutDepths returns an error, use
							*	GetErrorPin to determine the related pin.
							*/
	uint32_t				GetErrorPin(void) const
								{return(mErrorPin);}
	const float*			GetCutDepths(void) const
								{return(mCutDepths);}
	bool					IsMetric(void) const
								{return (mSpec.isMetric);}

	void					ResetMoveIndex(void)
								{mMoveIndex = 0;}
							/*
							*	Returns false if there are no more moves.
							*	Each cut is up to 4 moves
							*/
	bool					NextMove(
								float&					outX,
								float&					outZ);
	uint32_t				GetMoveIndex(void) const
								{return(mMoveIndex);}
protected:
	SKeySpec			mSpec;
#ifndef __MACH__
	StepControl*		mController;
	Stepper*			mXStepper;
	Stepper*			mZStepper;
#endif
	int32_t				mOriginX;
	int32_t				mOriginZ;
	float				mCutAngleIntersectionX;
	float				mCutAngleIntersectionZ;
	float				m1stFlatLeft;	// Distance from shoulder to first flat
	float				mFlatSpacing;	// Distance between (whole) flats
	uint32_t			mPinCount;
	float				mCutDepths[SKeySpec::eMaxPinCount+1];	// +1 for dummy end pin
	float				mLastX;
	float				mLastZ;
	uint32_t			mMoveIndex;
	uint32_t			mErrorPin;
	static const char	kName[];

	bool					NextIntersection(
								float&					outX,
								float&					outZ);
};

#endif /* CutKey_h */
