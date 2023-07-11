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
  /* 低电压安全锁，仅可用过RESET解除 */
  if (IsMainPwrLowVoltLocked)
  {
    LF.Speed(0);
    RF.Speed(0);
    LB.Speed(0);
    RB.Speed(0);
    digitalWrite(MOTOR_SAFETY_PIN, LOW);       // 锁定电机
    digitalWrite(LIDAR_ROTATE_MOTOR_PWM, HIGH); // 雷达电机关闭

    for (int i = 0; i < 5 * 5; i++)            // 声音报警
    {
      BEEP_ON();
      delay(100);
      BEEP_OFF();
      delay(100);
    }

    digitalWrite(PWR_5V_EN, LOW); // 关闭5V外设主电源
    
    /* 停止中断处理 */
    detachInterrupt(BUTTON_PIN);
    ITimer.detachInterrupt();
    ITimer.disableTimer();
    
    while(1);                     // 阻塞主循环
  }

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
    ISR_Timer.setTimer( // 灵活使用定时器进行异步延时，不要长时间阻塞主循环
        100, []()
        { BEEP_OFF(); },
        1);
  }

  if (flag_DoReportVoltage)
  {
    TransDataBuf pack;
    pack.data.HEADER = HEADER_MARK;
    pack.data.type = RPT__MAIN_BAT_VOLT;
    pack.data.data[0] = (int16_t)(voltage * 1000);
    UpSerial.write(pack.bytes, sizeof(TransDataBuf));
    if (voltage < MAIN_PWR_VOTAGE_LOW_LIMIT)
    {
      IsMainPwrLowVoltLocked = 1;
    }
    flag_DoReportVoltage = 0;
  }
}