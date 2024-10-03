#pragma once
//#define USE_ACC_TIMER	1
#define USE_PWM_PULSE_TIMER	1
namespace TeensyStep
{
    struct TF_Handler
    {
        virtual void stepTimerISR() = 0;
#ifdef USE_ACC_TIMER
        virtual void accTimerISR() = 0;
#endif
#ifndef USE_PWM_PULSE_TIMER
       virtual void pulseTimerISR() = 0;
#endif
    };
} // namespace TeensyStep