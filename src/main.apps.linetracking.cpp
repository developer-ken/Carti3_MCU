#include <Arduino.h>
#include "main.apps.linetracking.h"
#include <STM32TimerInterrupt.h>
#include <STM32_ISR_Timer.hpp>
#include <Adafruit_NeoPixel.h>
#include "config.h"
#include "beep.h"

#define SPEED_MAX (7)     //电机最大电压
#define SPEED_TURN (3.5)  //电机转向电压
#define SPEED_DIFF (2)    //电机修正电压差
#define HALF_LEN_T (250)  //路口延时（向前走出半个车长度的时间）

#define SPEED_STD (SPEED_MAX - SPEED_DIFF)

extern STM32_ISR_Timer ISR_Timer;
extern float voltage;
extern Adafruit_NeoPixel strip;
int timer_triggered = 0; // 0=none,1=l,2=r

LineTracking::LineTracking(ILineTrackingSensor *sensor, IMotor *lf, IMotor *rf, IMotor *lb, IMotor *rb)
{
    this->sensor = sensor;
    this->LF = lf;
    this->RF = rf;
    this->LB = lb;
    this->RB = rb;
    // this->lastresult = Loss;
    this->sensor->LineTrackingStateChangedEvent =
        std::bind(&LineTracking::lineTrackingSensorCallback, this, std::placeholders::_1);
    state = WorkingState::WStraight;
    CmdAtNextCrossing = CrossingCommand::GoStraight;
}

void LineTracking::lineTrackingSensorCallback(LineTrackingResult ltr)
{
    // lt状态机
    switch (state) // 当前状态
    {
    case WorkingState::WStraight:
    case WorkingState::WCorrLeft:
    case WorkingState::WCorrRight:
    {
        switch (ltr) // 状态转出条件
        {
        case LineTrackingResult::Crossing:
        {
            state = Stop;
            switch (CmdAtNextCrossing)
            {
            case CrossingCommand::Left:
            {
                state = LPending;
                break;
            }
            case CrossingCommand::Right:
            {
                state = RPending;
                break;
            }
            case CrossingCommand::GoStraight:
            {
                state = StPending;
                break;
            }
            case CrossingCommand::CStop:
            {
                state = Stop;
                break;
            }
            }
            this->CommandExecuted = true;
            break;
        }

        case LineTrackingResult::CorrLeft:
        {
            state = WCorrLeft;
            break;
        }

        case LineTrackingResult::CorrRight:
        {
            state = WCorrRight;
            break;
        }
        }
    }
    break;

    case WorkingState::LPending:
    {
        if (ltr != LineTrackingResult::Crossing)
        {
            ISR_Timer.setTimer(
                HALF_LEN_T, []()
                { timer_triggered = 1; },
                1);
        }
    }
    break;

    case WorkingState::RPending:
    {
        if (ltr != LineTrackingResult::Crossing)
        {
            ISR_Timer.setTimer(
                HALF_LEN_T, []()
                { timer_triggered = 2; },
                1);
        }
    }
    break;

    case WorkingState::StPending:
    {
        if (ltr != LineTrackingResult::Crossing)
        {
            state = WStraight;
            // ISR_Timer.setTimer(
            //     HALF_LEN_T, []()
            //     { timer_triggered = 3; },
            //     1);
        }
    }
    break;

    case WorkingState::TurnLeft:
    case WorkingState::TurnRight:
    {
        if (!this->sensor->isMiddleSensorTriggered)
        {
            state = NormPending;
        }
    }
    break;

    case WorkingState::NormPending:
    {
        if (this->sensor->isMiddleSensorTriggered)
        {
            state = WStraight;
        }
    }
    break;
    }
}

int LineTracking::Update(CrossingCommand *cmdQueue, int cmdQueueLength)
{
    sensor->TriggerOverallUpdate();
    static bool willstopatnxtc = false;
    static int cmdindex = 0;
    if (timer_triggered)
    {
        if (timer_triggered == 1)
        {
            state = TurnLeft;
        }
        else if (timer_triggered == 2)
        {
            state = TurnRight;
        }
        else if (timer_triggered == 3)
        {
            state = WStraight;
        }
        timer_triggered = 0;
    }
    if (CommandExecuted)
    {
        CommandExecuted = false;
        // 到路口了，喂进去下一个路口指令
        if (cmdindex < cmdQueueLength)
        {
            while (cmdQueue[cmdindex] == Alarm)
            {
                cmdindex++;
                BEEP_ON();
                strip.setPixelColor(0, 255, 0, 0);
                strip.setPixelColor(1, 255, 0, 0);
                strip.show();
            }
            CmdAtNextCrossing = cmdQueue[cmdindex];
            cmdindex++;
        }
        else
        {
            CmdAtNextCrossing = CrossingCommand::CStop;
        }
        //BEEP_ON();
        //delay(50);
        //BEEP_OFF();
    }
    switch (state) // 根据状态控制电机
    {
    case WorkingState::WStraight:
    case WorkingState::LPending:
    case WorkingState::RPending:
    case WorkingState::StPending:
    {
        LF->Speed(SPEED_STD * 255 / voltage);
        RF->Speed(SPEED_STD * 255 / voltage);
        LB->Speed(SPEED_STD * 255 / voltage);
        RB->Speed(SPEED_STD * 255 / voltage);
        break;
    }
    case WorkingState::WCorrLeft:
    {
        LF->Speed((SPEED_STD - SPEED_DIFF) * 255 / voltage);
        RF->Speed((SPEED_STD + SPEED_DIFF) * 255 / voltage);
        LB->Speed((SPEED_STD - SPEED_DIFF) * 255 / voltage);
        RB->Speed((SPEED_STD + SPEED_DIFF) * 255 / voltage);
        break;
    }
    case WorkingState::WCorrRight:
    {
        LF->Speed((SPEED_STD + SPEED_DIFF) * 255 / voltage);
        RF->Speed((SPEED_STD - SPEED_DIFF) * 255 / voltage);
        LB->Speed((SPEED_STD + SPEED_DIFF) * 255 / voltage);
        RB->Speed((SPEED_STD - SPEED_DIFF) * 255 / voltage);
        break;
    }
    case WorkingState::TurnLeft:
    {
        LF->Speed((-SPEED_TURN) * 255 / voltage);
        RF->Speed((SPEED_TURN)*255 / voltage);
        LB->Speed((-SPEED_TURN) * 255 / voltage);
        RB->Speed((SPEED_TURN)*255 / voltage);
        break;
    }
    case WorkingState::TurnRight:
    {
        LF->Speed((SPEED_TURN)*255 / voltage);
        RF->Speed((-SPEED_TURN) * 255 / voltage);
        LB->Speed((SPEED_TURN)*255 / voltage);
        RB->Speed((-SPEED_TURN) * 255 / voltage);
        break;
    }
    case WorkingState::Stop:
    {
        LF->Speed(0);
        RF->Speed(0);
        LB->Speed(0);
        RB->Speed(0);
        break;
    }
    }
    return cmdindex;
}