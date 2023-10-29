#ifdef MOTOR_DRIVER_6612
#include <Arduino.h>
#include <motor_tb6612.h>

MotorTb6612::MotorTb6612(PinName CW, PinName CCW, PinName PWM)
{
    cw = CW;
    ccw = CCW;
    pwm = PWM;
}

void MotorTb6612::begin()
{
    pinMode(cw, OUTPUT);
    pinMode(ccw, OUTPUT);
    pinMode(pwm, OUTPUT);
    analogWrite(pwm, 0);
}

void MotorTb6612::Speed(int32_t speed)
{
    if (speed > 0)
    {
        digitalWrite(cw, HIGH);
        digitalWrite(ccw, LOW);
        analogWrite(pwm, speed);
    }
    else if (speed < 0)
    {
        digitalWrite(cw, LOW);
        digitalWrite(ccw, HIGH);
        analogWrite(pwm, -speed);
    }
    else
    {
        digitalWrite(cw, LOW);
        digitalWrite(ccw, LOW);
        analogWrite(pwm, 0);
    }
}
#endif