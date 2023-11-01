#ifdef MOTOR_DRIVER_TB6612
#ifndef _H_LIBMOTOR_MOTOR_TB6612_
#define _H_LIBMOTOR_MOTOR_TB6612_
#include <Arduino.h>
#include "motor.h"
class MotorTb6612 : public IMotor
{
public:
    MotorTb6612(PinName CW, PinName CCW, PinName PWM, bool invert = false);

    void begin();

    /// @brief Change the motor sppeed.
    /// @param speed -255~255
    void Speed(int32_t speed);

private:
    PinName cw, ccw, pwm;
    bool Invert;
};
#endif
#endif