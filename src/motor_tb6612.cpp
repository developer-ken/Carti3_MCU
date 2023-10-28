#ifdef MOTOR_DRIVER_6612
#include <Arduino.h>
#include <motor_tb6612.h>

Motor::Motor(PinName CW, PinName CCW, PinName PWM)
{
    cw = CW;
    ccw = CCW;
    pwm = PWM;
}

void Motor::begin()
{
    pinMode(cw, OUTPUT);
    pinMode(ccw, OUTPUT);
    pinMode(pwm, OUTPUT);
    analogWrite(pwm, 0);
}

void Motor::Speed(int32_t speed)
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