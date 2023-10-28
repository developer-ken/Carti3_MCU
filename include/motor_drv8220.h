#ifdef MOTOR_DRIVER_DRV8220
#ifndef _H_LIBMOTOR_MOTOR_
#define _H_LIBMOTOR_MOTOR_
#include <Arduino.h>
class Motor
{
public:
    Motor(PinName PH, PinName PWM);

    void begin();

    /// @brief Change the motor sppeed.
    /// @param speed -255~255
    void Speed(int32_t speed);

private:
    PinName ph, pwm;
};
#endif
#endif