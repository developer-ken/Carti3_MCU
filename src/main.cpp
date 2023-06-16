#include "main.h"

void setup()
{
  INIT_BEEP();
  Init_Motors();
  Init_Button();
  Init_5V_Pwr();
  Init_Others();
#ifndef SKIP_SELFTEST_ON_BOOT
  SelfTest();
#endif
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
    ISR_Timer.setTimer(
        100, []()
        { BEEP_OFF(); },
        1);
  }
}