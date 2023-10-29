#ifndef _H_LIBMOTOR_MOTOR_INTERFACE_
#define _H_LIBMOTOR_MOTOR_INTERFACE_
#include <Arduino.h>
class IMotor
{
public:
    virtual void begin() = 0;

    /// @brief Change the motor sppeed.
    /// @param speed -255~255
    virtual void Speed(int32_t speed) = 0;
};
#endif