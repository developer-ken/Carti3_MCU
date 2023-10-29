#ifndef _H_MAIN_
#define _H_MAIN_
#include <Arduino.h>
#include <Adafruit_NeoPixel.h>

#ifdef STM32F1  // 主板使用STM32F103CXT6时的配置

#include <STM32TimerInterrupt.h>
#include <STM32_ISR_Timer.hpp>

#define SERIAL_RX PA_10
#define SERIAL_TX PA_9

#define AUX_SERIAL_RX PB_11
#define AUX_SERIAL_TX PB_10

#define MOTOR_LB_PWM PA_0
#define MOTOR_LB_CW PB_5

#define MOTOR_LF_PWM PA_1
#define MOTOR_LF_CW PB_4

#define MOTOR_RF_PWM PA_2
#define MOTOR_RF_CW PB_3

#define MOTOR_RB_PWM PA_3
#define MOTOR_RB_CW PA_15

#define FSENSOR_P0 PA_12
#define FSENSOR_P1 PA_11
#define FSENSOR_P2 PA_8
#define FSENSOR_P3 PB_15
#define FSENSOR_P4 PB_14
#define FSENSOR_P5 PB_13

#define BEEP_PIN PB_1
#define ADDR_LED_PIN PA_4
#define  MOTOR_SAFETY_PIN PB_7

#define LIDAR_ROTATE_MOTOR_PWM PB_6

#define BUTTON_PIN_1 PC14
#define BUTTON_PIN_2 PC13

#define PWR_5V_EN PC15
#define BAT_VSENSE PA7

#define MAIN_PWR_VSENSE_FACTOR 0.01405f
#define MAIN_PWR_VOTAGE_LOW_LIMIT 10.6f

#endif

// #define SKIP_SELFTEST_ON_BOOT  //禁止开机自检(测试RGB灯及蜂鸣器)
// #define DISABLE_BEEP // 禁止蜂鸣器鸣响

#define HEADER_MARK 0xAF

#include "beep.h"
#include "motor_tb6612.h"
#include "motor_drv8220.h"
#include "linetracking_5way.h"
#include "main.apps.linetracking.h"

enum PackType : uint8_t
{
    MOTOR_CMD = 0xF1,
    LED_COLOR_CHANGE = 0xF2,
    BEEP_CMD = 0xF3,
    MOTOR_LOCK_UNLOCK = 0xF4,

    RPT__MAIN_BAT_VOLT = 0xC1,
    RPT__BUTTON_DOWN = 0xC2,
    RPT__BUTTON_UP = 0xC3
};

struct TransData
{
    uint8_t HEADER;
    PackType type;
    int16_t data[4];
};

union TransDataBuf
{
    TransData data;
    uint8_t bytes[10];
};

#ifndef MAIN_FUNC_CPP
#include <STM32_ISR_Timer-Impl.h>
extern HardwareSerial UpSerial;
extern HardwareSerial DebugSerial;

extern LinetrackingSensor5W ltsensor;
extern LineTracking linetracking;

#ifdef MOTOR_DRIVER_TB6612
extern MotorTb6612 LF, RF, LB, RB;
#endif
#ifdef MOTOR_DRIVER_DRV8220
extern MotorDrv8220 LF, RF, LB, RB;
#endif

extern Adafruit_NeoPixel strip;
extern STM32Timer ITimer;
extern STM32_ISR_Timer ISR_Timer;
extern int8_t vvpointer;
extern uint8_t WButtonPressed;
extern TransDataBuf transbuffer;
extern float voltage;
extern uint8_t flag_DoReportVoltage;
extern uint8_t IsMainPwrLowVoltLocked;
#endif

void TimerHandler();
void ButtonPressHandler();
void Init_Motors();
void Init_Button();
void Init_5V_Pwr();
void Init_Others();
void HandleCommand();

void ReportVotage();
void SampleVotage();

#ifndef SKIP_SELFTEST_ON_BOOT
void SelfTest();
#endif

#endif