#ifdef MOTOR_DRIVER_DRV8220
#include <Arduino.h>
#include <motor_drv8220.h>

MotorDrv8220::MotorDrv8220(PinName PH, PinName PWM, bool invert)
{
    ph = PH;
    pwm = PWM;
    Invert = invert;
}

void MotorDrv8220::begin()
{
    pinMode(ph, OUTPUT);
    pinMode(pwm, OUTPUT);
    analogWrite(pwm, 0);
}

void MotorDrv8220::Speed(int32_t speed)
{
    if (speed > 0)
    {
        digitalWrite(ph, Invert ? LOW : HIGH);
        analogWrite(pwm, speed);
    }
    else if (speed < 0)
    {
        digitalWrite(ph, Invert ? HIGH : LOW);
        analogWrite(pwm, -speed);
    }
    else
    {
        digitalWrite(ph, LOW);
        analogWrite(pwm, 0);
    }
}
#endif