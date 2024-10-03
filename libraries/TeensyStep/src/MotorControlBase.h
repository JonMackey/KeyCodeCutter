#pragma once

#include "ErrorHandler.h"
#include "timer/TF_Handler.h"
#include "Stepper.h"

namespace TeensyStep{

	enum ErrCode {
		err_OK,
		err_movment_not_possible,
		err_too_much_motors
	};

	using ErrFunc = void (*)(ErrCode);

	constexpr int MaxMotors = 4;

	template <typename TimerField>
	class MotorControlBase : TF_Handler, ErrorHandler
	{
	 public:
		void begin(void);
		bool isOk() const { return OK; }
		bool isRunning() const;
		bool isAllocated() const;
		int getCurrentSpeed() const;
		uint32_t getPulseWidth() const {return(pulseWidth);}

		void emergencyStop() { timerField.end(); }

		  // set callback function to be called when target is reached
		void setCallback(void (*_callback)()) { this->callback = _callback; }

		virtual ~MotorControlBase();

		void attachErrorFunction(ErrFunc ef) { errFunc = ef; }
		TimerField& getTimerField(void) {return(timerField);}

	 protected:
		TimerField timerField;
		MotorControlBase(unsigned pulseWidth, unsigned accUpdatePeriod);

		template <size_t N>
		void attachStepper(Stepper* (&motors)[N]);
		template <typename... Steppers>
		void attachStepper(Stepper& stepper, Steppers&... steppers);
		void attachStepper(Stepper& stepper);

 //		  void stepTimerISR();
#ifndef USE_PWM_PULSE_TIMER
		void pulseTimerISR();
#endif
		Stepper* motorList[MaxMotors + 1];
		Stepper* leadMotor;

		void (*callback)() = nullptr;
		ErrFunc errFunc = nullptr;
		inline void Error(ErrCode e)
		{
			if (errFunc != nullptr) errFunc(e);
		}

		bool OK = false;

		unsigned mCnt;

		enum class Mode {
			target,
			notarget
		} mode = Mode::notarget;

#ifdef USE_ACC_TIMER
	   uint32_t accUpdatePeriod;
#endif
		uint32_t pulseWidth;

		inline mcErr err(mcErr code) const { return (mcErr)error(errModule::MC, (int)code); }

		MotorControlBase(const MotorControlBase&) = delete;
		MotorControlBase& operator=(const MotorControlBase&) = delete;
	};

	// Implementation ============================================================================

	template <typename t>
	bool MotorControlBase<t>::isRunning() const
	{
		return timerField.stepTimerIsRunning();
	}

	template <typename t>
	bool MotorControlBase<t>::isAllocated() const
	{
		return timerField.stepTimerIsAllocated();
	}

	template <typename t>
	int MotorControlBase<t>::getCurrentSpeed() const
	{
		return timerField.getStepFrequency();
	}

	template <typename t>
	MotorControlBase<t>::MotorControlBase(unsigned pulseWidth, unsigned accUpdatePeriod)
		: timerField(this), mCnt(0)
	{
	#ifndef USE_PWM_PULSE_TIMER
		this->pulseWidth = pulseWidth;
	#endif
	#ifdef USE_ACC_TIMER
		this->accUpdatePeriod = accUpdatePeriod;
	#endif
	}

	template <typename t>
	MotorControlBase<t>::~MotorControlBase()
	{
		if (OK)
			emergencyStop();
	}
	
	template <typename t>
	void MotorControlBase<t>::begin(void)
	{
	#ifndef USE_PWM_PULSE_TIMER
		timerField.setPulseWidth(pulseWidth);
	#endif
	#ifdef USE_ACC_TIMER
		timerField.setAccUpdatePeriod(accUpdatePeriod);
	#endif
	}
#if 0	// stepTimerISR moved to StepControllerBase.h
	template <typename t>
	void MotorControlBase<t>::stepTimerISR()
	{
		leadMotor->doStep(); // move master motor

		Stepper** slave = motorList;
		while (*(++slave) != nullptr) // move slave motors if required (https://en.wikipedia.org/wiki/Bresenham)
		{
			if ((*slave)->B >= 0)
			{
				(*slave)->doStep();
				(*slave)->B -= leadMotor->A;
			}
			(*slave)->B += (*slave)->A;
		}
		timerField.triggerDelay(); // start delay line to dactivate all step pins

		if (mode == Mode::target && (leadMotor->current == leadMotor->target)) // stop timer and call callback if we reached target
		{
			timerField.stepTimerStop(); // Was commented out.
			if (callback != nullptr)
				callback();
		}
	}
#endif
#ifndef USE_PWM_PULSE_TIMER
	template <typename t>
	void MotorControlBase<t>::pulseTimerISR()
	{
		Stepper** motor = motorList;
		while ((*motor) != nullptr)
		{
			(*motor++)->clearStepPin();
		}
	}
#endif
	template <typename t>
	void MotorControlBase<t>::attachStepper(Stepper& stepper)
	{
		motorList[mCnt++] = &stepper;
		motorList[mCnt] = nullptr;
		mCnt = 0;
	}

	template <typename t>
	template <typename... Steppers>
	void MotorControlBase<t>::attachStepper(Stepper& stepper, Steppers&... steppers)
	{
		static_assert(sizeof...(steppers) < MaxMotors, "Too many motors used. Please increase MaxMotors in file MotorControlBase.h");

		motorList[this->mCnt++] = &stepper;
		attachStepper(steppers...);
	}

	template <typename t>
	template <size_t N>
	void MotorControlBase<t>::attachStepper(Stepper* (&motors)[N])
	{
		static_assert(N <= MaxMotors, "Too many motors used. Please increase MaxMotors in file MotorControlBase.h");

		for (size_t i = 0; i < N; i++)
		{
			this->motorList[i] = motors[i];
		}
		this->motorList[N] = nullptr;
	}

} // namespace TeensyStep