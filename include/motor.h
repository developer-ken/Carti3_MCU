#ifndef _H_LIBMOTOR_MOTOR_
#define _H_LIBMOTOR_MOTOR_
#include <Arduino.h>
class Motor
{
public:
    Motor(PinName CW, PinName CCW, PinName PWM);

    void begin();

    /// @brief Change the motor sppeed.
    /// @param speed -255~255
    void Speed(int32_t speed);

private:
    PinName cw, ccw, pwm;
};
#endif