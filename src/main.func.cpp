#define MAIN_FUNC_CPP
#include "main.h"

HardwareSerial UpSerial(SERIAL_RX, SERIAL_TX);
Motor LF(MOTOR_LF_CW, MOTOR_LF_CCW, MOTOR_LF_PWM),
    RF(MOTOR_RF_CW, MOTOR_RF_CCW, MOTOR_RF_PWM),
    LB(MOTOR_LB_CW, MOTOR_LB_CCW, MOTOR_LB_PWM),
    RB(MOTOR_RB_CW, MOTOR_RB_CCW, MOTOR_RB_PWM);
Adafruit_NeoPixel strip = Adafruit_NeoPixel(2, ADDR_LED_PIN, NEO_GRB + NEO_KHZ800);

STM32Timer ITimer(TIM1);
STM32_ISR_Timer ISR_Timer;

int8_t vvpointer = -1;
volatile uint8_t isButtonPressed = 0;
TransDataBuf transbuffer;

void TimerHandler()
{
    ISR_Timer.run();
}

void ButtonPressHandler()
{
    BEEP_ON();
    isButtonPressed = 1;
}

void Init_Motors()
{
    LF.begin();
    RF.begin();
    LB.begin();
    RB.begin();
    RB.Speed(0);
    RF.Speed(0);
    LB.Speed(0);
    LF.Speed(0);
    pinMode(MOTOR_SAFETY_PIN, OUTPUT);
    digitalWrite(MOTOR_SAFETY_PIN, LOW); // 锁定电机，防止误动作
}

void Init_Button()
{
    pinMode(BUTTON_PIN, INPUT_PULLUP);
    attachInterrupt(digitalPinToInterrupt(BUTTON_PIN), ButtonPressHandler, FALLING);
}

void Init_5V_Pwr()
{
    pinMode(PWR_5V_EN, OUTPUT);
    digitalWrite(PWR_5V_EN, HIGH); // 5V外设主电源使能
}

void Init_Others()
{
    UpSerial.begin(115200);
    strip.begin();
    strip.setBrightness(255);
    ITimer.attachInterruptInterval(50 * 50, TimerHandler);
    // 50为常数，第二个50为中断周期(ms)。之后挂在ISR_Timer上的函数延迟和周期必须是50ms的整倍数。
}

#ifndef SKIP_SELFTEST_ON_BOOT
void SelfTest()
{
    { // RGB & Beep Test
        BEEP_ON();
        strip.setPixelColor(0, 255, 0, 0);
        strip.setPixelColor(1, 255, 0, 0);
        strip.show();
        delay(250);
        BEEP_OFF();
        strip.setPixelColor(0, 0, 0, 0);
        strip.setPixelColor(1, 0, 0, 0);
        strip.show();
        delay(250);
        BEEP_ON();
        strip.setPixelColor(0, 0, 255, 0);
        strip.setPixelColor(1, 0, 255, 0);
        strip.show();
        delay(250);
        BEEP_OFF();
        strip.setPixelColor(0, 0, 0, 0);
        strip.setPixelColor(1, 0, 0, 0);
        strip.show();
        delay(250);
        BEEP_ON();
        strip.setPixelColor(0, 0, 0, 255);
        strip.setPixelColor(1, 0, 0, 255);
        strip.show();
        delay(250);
        BEEP_OFF();
        strip.setPixelColor(0, 0, 0, 0);
        strip.setPixelColor(1, 0, 0, 0);
        strip.show();
        delay(250);
        BEEP_ON();
        strip.setPixelColor(0, 255, 255, 255);
        strip.setPixelColor(1, 255, 255, 255);
        strip.show();
        delay(500);
        BEEP_OFF();
        strip.setPixelColor(0, 0, 0, 0);
        strip.setPixelColor(1, 0, 0, 0);
        strip.show();
    }
}
#endif