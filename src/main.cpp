#include "main.h"

void setup()
{
  INIT_BEEP();
  Init_5V_Pwr();
  Init_Motors();
  Init_Button();
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
    digitalWrite(MOTOR_SAFETY_PIN, LOW);        // 锁定电机
    digitalWrite(LIDAR_ROTATE_MOTOR_PWM, HIGH); // 雷达电机关闭

    for (int i = 0; i < 5 * 5; i++) // 声音报警
    {
      BEEP_ON();
      delay(100);
      BEEP_OFF();
      delay(100);
    }

    digitalWrite(PWR_5V_EN, LOW); // 关闭5V外设主电源

    /* 停止中断处理 */
    detachInterrupt(BUTTON_PIN_1);
    detachInterrupt(BUTTON_PIN_2);
    ITimer.detachInterrupt();
    ITimer.disableTimer();

    while (1)
      ; // 阻塞主循环
  }

  

  if (WButtonPressed)
  {
    WButtonPressed = 0;
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