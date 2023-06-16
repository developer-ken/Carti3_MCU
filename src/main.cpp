#include <Arduino.h>
#include <Adafruit_NeoPixel.h>
#include <STM32TimerInterrupt.h>
#include <STM32_ISR_Timer.h>

#define BEEP_PIN PB_1
#define ADDR_LED_PIN PA_0
#define MOTOR_SAFETY_PIN PB_7
#define PWR_5V_EN PC_13
#define BAT_VSENSE PB_0
#define BUTTON_PIN PA_7

#include "beep.h"
#include "motor.h"

HardwareSerial UpSerial(PA_10, PA_9);
Motor LF(PA_3, PA_4, PA_1),
    RF(PB_4, PB_5, PB_8),
    LB(PA_6, PA_5, PA_2),
    RB(PB_3, PA_15, PB_9);
Adafruit_NeoPixel strip = Adafruit_NeoPixel(2, ADDR_LED_PIN, NEO_GRB + NEO_KHZ800);

#define HEADER_MARK 0xAF

STM32Timer ITimer(TIM1);
STM32_ISR_Timer ISR_Timer;

int8_t vvpointer = -1;
volatile uint8_t isButtonPressed = 0;

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
} transbuffer;

enum PackType
{
  MOTOR_CMD = 0xF1,
  LED_COLOR_CHANGE = 0xF2,
  BEEP_CMD = 0xF3,
  MOTOR_LOCK_UNLOCK = 0xF4,
};

void TimerHandler()
{
  ISR_Timer.run();
}

void ButtonPressHandler()
{
  BEEP_ON();
  isButtonPressed = 1;
}

void setup()
{
  INIT_BEEP();
  LF.begin();
  RF.begin();
  LB.begin();
  RB.begin();
  RB.Speed(0);
  RF.Speed(0);
  LB.Speed(0);
  LF.Speed(0);
  pinMode(BUTTON_PIN, INPUT_PULLUP);
  attachInterrupt(digitalPinToInterrupt(BUTTON_PIN), ButtonPressHandler, FALLING);
  pinMode(PWR_5V_EN, OUTPUT);
  pinMode(MOTOR_SAFETY_PIN, OUTPUT);
  digitalWrite(MOTOR_SAFETY_PIN, LOW); // 锁定电机，防止误动作
  digitalWrite(PWR_5V_EN, HIGH);       // 5V功率供电使能
  UpSerial.begin(115200);
  strip.begin();
  strip.setBrightness(255);
  
  ITimer.attachInterruptInterval(50 * 50, TimerHandler);
  // 50为常数，第二个50为中断周期(ms)。之后挂在ISR_Timer上的函数延迟和周期必须是50ms的整倍数。
  
  { // RGB Test
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

void loop()
{
  if (UpSerial.available() == 10)
  {
    UpSerial.readBytes(transbuffer.bytes, 10);
    if (transbuffer.data.HEADER != HEADER_MARK)
      return; // Recv failed

    switch (transbuffer.data.type)
    {
    case MOTOR_CMD:
      LF.Speed(transbuffer.data.data[0]);
      RF.Speed(transbuffer.data.data[1]);
      LB.Speed(transbuffer.data.data[2]);
      RB.Speed(transbuffer.data.data[3]);
      break;
    case LED_COLOR_CHANGE:
      strip.setPixelColor(transbuffer.data.data[0], transbuffer.data.data[1], transbuffer.data.data[2], transbuffer.data.data[3]);
      strip.show();
      break;
    case BEEP_CMD:
      if (transbuffer.data.data[0] == 1)
        BEEP_ON();
      else
        BEEP_OFF();
      break;
    case MOTOR_LOCK_UNLOCK:
      if (transbuffer.data.data[0] == 1)
        digitalWrite(MOTOR_SAFETY_PIN, HIGH);
      else
        digitalWrite(MOTOR_SAFETY_PIN, LOW);
      break;
    }
  }

  if (isButtonPressed)
  {
    isButtonPressed = 0;
    ISR_Timer.setTimer(100, []() { BEEP_OFF(); },1);
  }
}