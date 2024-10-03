#include <Arduino.h>
#include "TeensyStep.h"


#pragma push_macro("abs")
#undef abs

namespace TeensyStep
{
	constexpr int32_t Stepper::vMaxMax;
	constexpr uint32_t Stepper::aMax;
	constexpr uint32_t Stepper::vMaxDefault;
	constexpr uint32_t Stepper::vPullInOutDefault;
	constexpr uint32_t Stepper::aDefault;
	constexpr uint32_t Stepper::kDefaultPulseWidth;

	Stepper::Stepper(const int _stepPin, const int _dirPin)
		: current(0), stepPin(_stepPin), dirPin(_dirPin)
	{
	#ifndef USE_PWM_PULSE_TIMER
		pinMode(stepPin, OUTPUT);	// Setup as output before calling setStepPinPolarity
	#endif	  
		pinMode(dirPin, OUTPUT);
		setStepPinPolarity(HIGH);
		setInverseRotation(false);
		setAcceleration(aDefault);
		setMaxSpeed(vMaxDefault);
		setPullInSpeed(vPullInOutDefault);

	}

	Stepper& Stepper::setStepPinPolarity(int polarity)
	{
#if defined(__MK20DX128__) || defined(__MK20DX256__) || defined(__MK64FX512__) || defined(__MK66FX1M0__)
		// Calculate adresses of bitbanded pin-set and pin-clear registers
		uint32_t pinRegAddr = (uint32_t)digital_pin_to_info_PGM[stepPin].reg; //GPIO_PDOR
		uint32_t* pinSetReg = (uint32_t*)(pinRegAddr + 4 * 32);				  //GPIO_PSOR = GPIO_PDOR + 4
		uint32_t* pinClearReg = (uint32_t*)(pinRegAddr + 8 * 32);			  //GPIO_PCOR = GPIO_PDOR + 8

		// Assign registers according to step option
		if (polarity == LOW)
		{
			stepPinActiveReg = pinClearReg;
			stepPinInactiveReg = pinSetReg;
		} else
		{
			stepPinActiveReg = pinSetReg;
			stepPinInactiveReg = pinClearReg;
		}
#else
		this->polarity = polarity;
#endif
		// ToDo: For PWM the polarity is set by calling LL_TIM_OC_ConfigOutput
		// This is currently only called in the constructor and it's assumed to
		// be high true.
#ifndef USE_PWM_PULSE_TIMER
		clearStepPin(); // set step pin to inactive state
#endif
		return *this;
	}
	
#ifdef USE_PWM_PULSE_TIMER
/****************************** setupPulseTimer *******************************/
bool Stepper::setupPulseTimer(
	unsigned pulseWidth)
{
	PinName pinName = digitalPinToPinName(stepPin);
	TIM_TypeDef*	timer = (TIM_TypeDef*)pinmap_peripheral(pinName, PinMap_TIM);
	bool success = STMTimers::Allocate(timer);
	uint32_t	timerParams = pinmap_find_function(pinName, PinMap_TIM);
	if (timer != nullptr &&
		timerParams)
	{
		uint32_t channel = STM_PIN_CHANNEL(timerParams);
		pulseTimer.setup(timer);
		/*
		*	Because HardwareTimer::setPWM expects a frequency, convert
		*	the pulseWidth to a frequency.	The PWM duty cycle when calling
		*	setPWM is 50%, so the supplied pulseWidth is half the frequency.
		*/
		uint32_t	timerClkFreq = pulseTimer.getTimerClkFreq();
		uint32_t	pwmFreq = timerClkFreq/(pulseWidth * (timerClkFreq / 500000));
		
		pulseTimer.setPWM(channel, pinName, pwmFreq, 50);
		int llChannel = pulseTimer.getLLChannel(channel);
		/*
		*	LL_TIM_OCIDLESTATE_LOW == 0
		*	setPWM() sets the polarity to high, low is needed to
		*	generate a high pulse.
		*/
		LL_TIM_OC_ConfigOutput(timer, llChannel, LL_TIM_OCPOLARITY_LOW);
		/*
		*	Set one pulse mode to get a single pulse whenever resume()
		*	is called.
		*
		*	Refresh needs to be called prior to calling resume()
		*	otherwise you get two pulses rather than one.
		*/
		LL_TIM_SetOnePulseMode(timer, LL_TIM_ONEPULSEMODE_SINGLE);
	}
	return(success);
}
#endif
	
	Stepper& Stepper::setInverseRotation(bool reverse)
	{
#if defined(__MK20DX128__) || defined(__MK20DX256__) || defined(__MK64FX512__) || defined(__MK66FX1M0__)
		// Calculate adresses of bitbanded pin-set and pin-clear registers
		uint32_t pinRegAddr = (uint32_t)digital_pin_to_info_PGM[dirPin].reg; //GPIO_PDOR
		uint32_t* pinSetReg = (uint32_t*)(pinRegAddr + 4 * 32);				 //GPIO_PSOR = GPIO_PDOR + 4
		uint32_t* pinClearReg = (uint32_t*)(pinRegAddr + 8 * 32);			 //GPIO_PCOR = GPIO_PDOR + 8

		if (reverse)
		{
			dirPinCwReg = pinClearReg;
			dirPinCcwReg = pinSetReg;
		} else
		{
			dirPinCwReg = pinSetReg;
			dirPinCcwReg = pinClearReg;
		}
#else
		this->reverse = reverse;
#endif
		return *this;
	}

	Stepper& Stepper::setAcceleration(uint32_t a) // steps/s^2
	{
		this->a = std::min(aMax, a);
		return *this;
	}

	Stepper& Stepper::setMaxSpeed(int32_t speed)
	{
		setDir(speed >= 0 ? 1 : -1);
		vMax = std::min(vMaxMax, std::max(-vMaxMax, speed));
		return *this;
	}

	Stepper& Stepper::setPullInSpeed(int32_t speed)
	{
		vPullIn = vPullOut = std::abs(speed);
		return *this;
	}

	Stepper& Stepper::setPullInOutSpeed(int32_t pullInSpeed, int32_t pullOutSpeed)
	{
		vPullIn = std::abs(pullInSpeed);
		vPullOut = std::abs(pullOutSpeed);

		return *this;
	}

	void Stepper::setTargetAbs(int32_t target)
	{
		setTargetRel(target - current);
	}

	void Stepper::setTargetRel(int32_t delta)
	{
		setDir(delta < 0 ? -1 : 1);
		target = current + delta;
		A = std::abs(delta);
	}
}

#pragma pop_macro("abs")
