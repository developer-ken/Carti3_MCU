#ifndef _H_MAIN_
#define _H_MAIN_
#include <Arduino.h>
#include <Adafruit_NeoPixel.h>

#ifdef STM32F1  // 主板使用STM32F103CXT6时的配置

#include <STM32TimerInterrupt.h>
#include <STM32_ISR_Timer.hpp>

#define SERIAL_RX PA_10
#define SERIAL_TX PA_9

#define MOTOR_LF_PWM PA_1
#define MOTOR_LF_CCW PA_4
#define MOTOR_LF_CW PA_3

#define MOTOR_RF_PWM PB_8
#define MOTOR_RF_CCW PB_5
#define MOTOR_RF_CW PB_4

#define MOTOR_LB_PWM PA_2
#define MOTOR_LB_CCW PA_5
#define MOTOR_LB_CW PA_6

#define MOTOR_RB_PWM PB_9
#define MOTOR_RB_CCW PA_15
#define MOTOR_RB_CW PB_3

#define BEEP_PIN PB_1
#define ADDR_LED_PIN PA_0
#define MOTOR_SAFETY_PIN PB_7
#define PWR_5V_EN PC_13
#define BAT_VSENSE PB_0
#define BUTTON_PIN PA_7

#endif


// #define SKIP_SELFTEST_ON_BOOT  //禁止开机自检(测试RGB灯及蜂鸣器)
#define DISABLE_BEEP // 禁止蜂鸣器鸣响

#define HEADER_MARK 0xAF

#include "beep.h"
#include "motor.h"

struct TransData
{
    uint8_t HEADER;
    uint8_t type;
    int16_t data[4];
};

union TransDataBuf
{
    TransData data;
    uint8_t bytes[10];
};

enum PackType
{
    MOTOR_CMD = 0xF1,
    LED_COLOR_CHANGE = 0xF2,
    BEEP_CMD = 0xF3,
    MOTOR_LOCK_UNLOCK = 0xF4,
};

#ifndef MAIN_FUNC_CPP
#include <STM32_ISR_Timer-Impl.h>
extern HardwareSerial UpSerial;
extern Motor LF, RF, LB, RB;
extern Adafruit_NeoPixel strip;
extern STM32Timer ITimer;
extern STM32_ISR_Timer ISR_Timer;
extern int8_t vvpointer;
extern uint8_t isButtonPressed;
extern TransDataBuf transbuffer;
#endif

void TimerHandler();
void ButtonPressHandler();
void Init_Motors();
void Init_Button();
void Init_5V_Pwr();
void Init_Others();

#ifndef SKIP_SELFTEST_ON_BOOT
void SelfTest();
#endif

#endif