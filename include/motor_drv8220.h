#ifdef MOTOR_DRIVER_DRV8220
#ifndef _H_LIBMOTOR_MOTOR_Drv8220_
#define _H_LIBMOTOR_MOTOR_Drv8220_
#include <Arduino.h>
#include "motor.h"
class MotorDrv8220 : public IMotor
{
public:
    MotorDrv8220(PinName PH, PinName PWM);

    void begin();

    /// @brief Change the motor sppeed.
    /// @param speed -255~255
    void Speed(int32_t speed);

private:
    PinName ph, pwm;
};
#endif
#endif