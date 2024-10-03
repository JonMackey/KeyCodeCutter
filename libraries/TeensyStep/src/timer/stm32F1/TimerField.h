#if defined(STM32F1xx)
#pragma once

#include <wiring.h>
#include <functional>

#include "../TF_Handler.h"
#include "STMTimers.h"

class TimerField
{
public:
  inline TimerField(TeensyStep::TF_Handler *_handler);

  inline bool begin();
  inline void end();
  inline void endAfterPulse(); // not nice look for better solution

  inline void stepTimerStart();
  inline void stepTimerStop();
  inline void setStepFrequency(unsigned f);
  inline unsigned getStepFrequency() { return stepTimer.getTimerClkFreq(); } //  clock of timer  or overflow?
  inline bool stepTimerIsRunning() const { return stepTimerRunning; }

#ifdef USE_ACC_TIMER
  //inline void accTimerStart() { accTimer.resume(); accTimer.refresh(); }
  inline void accTimerStart() { accTimer.refresh(); accTimer.resume(); }
  inline void accTimerStop() { accTimer.pause(); }
  inline void setAccUpdatePeriod(unsigned period);
#endif
  inline void triggerDelay();
  inline void setPulseWidth(unsigned pulseWidth);
  HardwareTimer&	getStepTimer(void) {return(stepTimer);}
#ifdef USE_ACC_TIMER
  HardwareTimer&	getAccTimer(void) {return(accTimer);}
#endif
#ifndef USE_PWM_PULSE_TIMER
  HardwareTimer&	getPulseTimer(void) {return(pulseTimer);}
#endif
protected:
  TeensyStep::TF_Handler *handler;
  HardwareTimer stepTimer;
#ifdef USE_ACC_TIMER
  HardwareTimer accTimer;
#endif
#ifndef USE_PWM_PULSE_TIMER
  HardwareTimer pulseTimer;
#endif
  volatile bool stepTimerRunning;
};
// TODO:
// * Known bug: Second interrupt always happens after 20ms, no matter what acceleration is set. This causes
// all other interrupts to be slightly delayed. With default acceleration the second interrupt happens
// after 20ms instead of after 11.6ms.
// All acceleration interrupts are shifted by one step,
//
// Example with 10 steps(ms, offset to each  other):
//  Teensy35 has steps: 0, 11.6, 9.0,  7.6, 6.6, 7.6, 9.0, 11.6, 20.0, 20.0
//  stm32f4 has steps : 0, 20.0, 11.6, 9.0, 7.6, 6.6, 7.5,  9.0, 11.6, 20.0
/*
	Update: The bug described above was happening for two reasons:
	- The step timer was not paused before setting the new step frequency. When
	refresh is called it forces the step ISR to be called.
	- The accTimer, by design, only updates the time periodically.  This
	essentially random update causes random step frequencies to be used.
			
	The fix removes the accTimer and its associated ISR.  The master step
	frequency is now updated on every call to the step ISR (only if changed.) 
	The step ISR has been moved to the StepControlBase.
	
	Other changes:
	- The common pulseTimer has been removed and replaced with individual PWM
	one-shot pulse timers owned by each Stepper.  To implement this get_timer()
	was renamed to AllocateNextAvailable and moved to STMTimers.h to make it
	generally available.
*/

// * Maximum 4 instances can be used, if more than four are initialized the first will break
// as its interrupt is reused.
// IMPLEMENTATION ====================================================================

TimerField::TimerField(TeensyStep::TF_Handler *_handler) :
     stepTimerRunning(false)
{
  handler = _handler;
}

void TimerField::stepTimerStart()
{
  stepTimer.resume();
  // Force update on the step timer registers as it might be slow(>50ms sometimes) to start otherwise.
  stepTimer.refresh();

  stepTimerRunning = true;
}

void TimerField::stepTimerStop()
{
	stepTimer.pause();
#ifdef USE_ACC_TIMER
	accTimer.pause();
#endif
	stepTimerRunning = false;
}

#ifdef USE_ACC_TIMER
void TimerField::setAccUpdatePeriod(unsigned period)
{
  accTimer.setOverflow(period, MICROSEC_FORMAT);
}
#endif

#ifndef USE_PWM_PULSE_TIMER
void TimerField::triggerDelay() {
  pulseTimer.resume();
}
void TimerField::setPulseWidth(unsigned pulseWidth)
{
  pulseTimer.setOverflow(pulseWidth, MICROSEC_FORMAT);
}
#endif

/****************************** setStepFrequency ******************************/
void TimerField::setStepFrequency(unsigned f)
{
	if (f)
	{
		stepTimer.pause();
		//					HAL_GPIO_WritePin(GPIOC, GPIO_PIN_11, GPIO_PIN_SET);
		//					HAL_GPIO_WritePin(GPIOC, GPIO_PIN_11, GPIO_PIN_RESET);
		stepTimer.setOverflow(f, HERTZ_FORMAT);
		stepTimer.resume();
#ifdef DebugSTMTimers
		STMTimers::PushFrequency(f);
#endif
	} else
	{
	    stepTimerStop();
	}
}

/************************************ begin ***********************************/
/*
*	Code moved from constructor so that other timers can be allocated by other
*	classes before timer allocation takes place here.
*/
bool TimerField::begin(void)
{
	if (stepTimer.getHandle()->Instance == nullptr)
	{
		stepTimer.setup(STMTimers::AllocateNextAvailable());
		stepTimer.attachInterrupt([this] { handler->stepTimerISR(); });
		stepTimer.setPreloadEnable(false);
	#ifdef USE_ACC_TIMER
		accTimer.setup(STMTimers::AllocateNextAvailable());
		accTimer.attachInterrupt([this] { handler->accTimerISR(); });
		accTimer.setPreloadEnable(false);
	#endif
	#ifndef USE_PWM_PULSE_TIMER
		pulseTimer.setup(STMTimers::AllocateNextAvailable());
		pulseTimer.attachInterrupt([this] { handler->pulseTimerISR(); this->pulseTimer.pause(); }); // one-shot mode
		pulseTimer.setPreloadEnable(false);
	#endif
	}
#ifdef DebugSTMTimers
	STMTimers::ClearFrequencies();
#endif
	return(stepTimer.getHandle()->Instance != nullptr);
}

void TimerField::end()
{
  stepTimer.pause();
#ifdef USE_ACC_TIMER
  accTimer.pause();
#endif
#ifndef USE_PWM_PULSE_TIMER
  pulseTimer.pause();
#endif
  stepTimerRunning = false;
}

void TimerField::endAfterPulse()
{
    //lastPulse = true;
}

#endif
