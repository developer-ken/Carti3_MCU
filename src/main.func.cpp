#define MAIN_FUNC_CPP
#include "main.h"

HardwareSerial UpSerial(SERIAL_RX, SERIAL_TX);
HardwareSerial DebugSerial(AUX_SERIAL_RX, AUX_SERIAL_TX);
#ifdef MOTOR_DRIVER_TB6612
Motor LF(MOTOR_LF_CW, MOTOR_LF_CCW, MOTOR_LF_PWM),
    RF(MOTOR_RF_CW, MOTOR_RF_CCW, MOTOR_RF_PWM),
    LB(MOTOR_LB_CW, MOTOR_LB_CCW, MOTOR_LB_PWM),
    RB(MOTOR_RB_CW, MOTOR_RB_CCW, MOTOR_RB_PWM);
#endif
#ifdef MOTOR_DRIVER_DRV8220
Motor LF(MOTOR_LF_CW, MOTOR_LF_PWM),
    RF(MOTOR_RF_CW, MOTOR_RF_PWM),
    LB(MOTOR_LB_CW, MOTOR_LB_PWM),
    RB(MOTOR_RB_CW, MOTOR_RB_PWM);
#endif

Adafruit_NeoPixel strip = Adafruit_NeoPixel(6, ADDR_LED_PIN, NEO_GRB + NEO_KHZ800);

STM32Timer ITimer(TIM1);
STM32_ISR_Timer ISR_Timer;

int8_t vvpointer = -1;
volatile uint8_t WButtonPressed = 0;
volatile uint8_t flag_DoReportVoltage = 0;
uint8_t IsMainPwrLowVoltLocked = 0;
TransDataBuf transbuffer;
float voltage;

void TimerHandler()
{
    ISR_Timer.run();
}

void ButtonPressHandler1()
{
    BEEP_ON();
    WButtonPressed = 1;
}

void ButtonPressHandler2()
{
    BEEP_ON();
    WButtonPressed = 2;
}

void MotorTest(){
    digitalWrite(MOTOR_SAFETY_PIN, HIGH);
    RF.Speed(255);
    RB.Speed(-255);

    LF.Speed(255);
    LB.Speed(255);
    while(1);
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
    //MotorTest();
}

void Init_Button()
{
    pinMode(BUTTON_PIN_1, INPUT_PULLUP);
    pinMode(BUTTON_PIN_2, INPUT_PULLUP);
    attachInterrupt(BUTTON_PIN_1, ButtonPressHandler1, FALLING);
    attachInterrupt(BUTTON_PIN_2, ButtonPressHandler2, FALLING);
}

void Init_5V_Pwr()
{
    pinMode(PWR_5V_EN, OUTPUT);
    digitalWrite(PWR_5V_EN, HIGH); // 5V外设主电源使能
}

void Init_Others()
{
    UpSerial.begin(115200);
    DebugSerial.begin(115200);
    strip.begin();
    strip.setBrightness(255);

    pinMode(LIDAR_ROTATE_MOTOR_PWM, OUTPUT);
    analogWrite(LIDAR_ROTATE_MOTOR_PWM, 255 / 2); // 雷达全速

    ITimer.attachInterruptInterval(50 * 50, TimerHandler);
    // 50为常数，第二个50为中断周期(ms)。之后挂在ISR_Timer上的函数延迟和周期必须是50ms的整倍数。

    ISR_Timer.setInterval(2000, ReportVotage);
    ISR_Timer.setInterval(100, SampleVotage);
}

void ReportVotage()
{
    flag_DoReportVoltage = 1; // 优先级低的工作不要阻塞中断，设置个标志位，放到主循环处理
}

void SampleVotage()
{
    float measure = (float)analogRead(BAT_VSENSE);
    voltage = (voltage + (measure * MAIN_PWR_VSENSE_FACTOR)) / 2;
}

#ifndef SKIP_SELFTEST_ON_BOOT
void SelfTest()
{
    { // RGB & Beep Test
        BEEP_ON();
        strip.setPixelColor(0, 255, 0, 0);
        strip.setPixelColor(1, 255, 0, 0);
        strip.setPixelColor(2, 255, 0, 0);
        strip.setPixelColor(3, 255, 0, 0);
        strip.setPixelColor(4, 255, 0, 0);
        strip.setPixelColor(5, 255, 0, 0);
        strip.show();
        delay(250);
        BEEP_OFF();
        strip.setPixelColor(0, 0, 0, 0);
        strip.setPixelColor(1, 0, 0, 0);
        strip.setPixelColor(2, 0, 0, 0);
        strip.setPixelColor(3, 0, 0, 0);
        strip.setPixelColor(4, 0, 0, 0);
        strip.setPixelColor(5, 0, 0, 0);
        strip.show();
        delay(250);
        BEEP_ON();
        strip.setPixelColor(0, 0, 255, 0);
        strip.setPixelColor(1, 0, 255, 0);
        strip.setPixelColor(2, 0, 255, 0);
        strip.setPixelColor(3, 0, 255, 0);
        strip.setPixelColor(4, 0, 255, 0);
        strip.setPixelColor(5, 0, 255, 0);
        strip.show();
        delay(250);
        BEEP_OFF();
        strip.setPixelColor(0, 0, 0, 0);
        strip.setPixelColor(1, 0, 0, 0);
        strip.setPixelColor(2, 0, 0, 0);
        strip.setPixelColor(3, 0, 0, 0);
        strip.setPixelColor(4, 0, 0, 0);
        strip.setPixelColor(5, 0, 0, 0);
        strip.show();
        delay(250);
        BEEP_ON();
        strip.setPixelColor(0, 0, 0, 255);
        strip.setPixelColor(1, 0, 0, 255);
        strip.setPixelColor(2, 0, 0, 255);
        strip.setPixelColor(3, 0, 0, 255);
        strip.setPixelColor(4, 0, 0, 255);
        strip.setPixelColor(5, 0, 0, 255);
        strip.show();
        delay(250);
        BEEP_OFF();
        strip.setPixelColor(0, 0, 0, 0);
        strip.setPixelColor(1, 0, 0, 0);
        strip.setPixelColor(2, 0, 0, 0);
        strip.setPixelColor(3, 0, 0, 0);
        strip.setPixelColor(4, 0, 0, 0);
        strip.setPixelColor(5, 0, 0, 0);
        strip.show();
        delay(250);
        BEEP_ON();
        strip.setPixelColor(0, 255, 255, 255);
        strip.setPixelColor(1, 255, 255, 255);
        strip.setPixelColor(2, 255, 255, 255);
        strip.setPixelColor(3, 255, 255, 255);
        strip.setPixelColor(4, 255, 255, 255);
        strip.setPixelColor(5, 255, 255, 255);
        strip.show();
        delay(500);
        BEEP_OFF();
        strip.setPixelColor(0, 0, 0, 0);
        strip.setPixelColor(1, 0, 0, 0);
        strip.setPixelColor(2, 0, 0, 0);
        strip.setPixelColor(3, 0, 0, 0);
        strip.setPixelColor(4, 0, 0, 0);
        strip.setPixelColor(5, 0, 0, 0);
        strip.show();
    }
}
#endif