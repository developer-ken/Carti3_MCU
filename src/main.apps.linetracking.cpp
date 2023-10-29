#include <Arduino.h>
#include "main.apps.linetracking.h"

#define SPEED_MAX 150
#define SPEED_DIFF 25

#define SPEED_STD SPEED_MAX - SPEED_DIFF

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
}

void LineTracking::lineTrackingSensorCallback(LineTrackingResult ltr)
{
    // lt状态机
    switch (state) // 当前状态
    {
    case WorkingState::WStraight:
    {
        switch (ltr) // 状态转出条件
        {
        case LineTrackingResult::Crossing:
        {
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
            }
            this->CommandExecuted = true;
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

    case WorkingState::WCorrLeft:
    {
        switch (ltr)
        {
        case LineTrackingResult::Crossing:
        {
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
            }
            this->CommandExecuted = true;
        }

        case LineTrackingResult::Straight:
        {
            state = WStraight;
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

    case WorkingState::WCorrRight:
    {
        switch (ltr)
        {
        case LineTrackingResult::Crossing:
        {
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
            }
            this->CommandExecuted = true;
        }

        case LineTrackingResult::Straight:
        {
            state = WStraight;
            break;
        }

        case LineTrackingResult::CorrLeft:
        {
            state = WCorrLeft;
            break;
        }
        }
    }
    break;

    case WorkingState::LPending:
    {
        if (ltr != LineTrackingResult::Crossing)
        {
            state = TurnLeft;
        }
    }
    break;

    case WorkingState::RPending:
    {
        if (ltr != LineTrackingResult::Crossing)
        {
            state = TurnRight;
        }
    }
    break;

    case WorkingState::StPending:
    {
        if (ltr != LineTrackingResult::Crossing)
        {
            state = WStraight;
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

void LineTracking::Update(HardwareSerial *serial)
{
    static bool willstopatnxtc = false;
    if (serial->available())
    {
        char c = serial->read();
        switch (c)
        {
        case 'l':
        {
            CmdAtNextCrossing = CrossingCommand::Left;
            break;
        }
        case 'r':
        {
            CmdAtNextCrossing = CrossingCommand::Right;
            break;
        }
        case 'e': // stop at next crossing
            willstopatnxtc = true;
        case 's':
        {
            CmdAtNextCrossing = CrossingCommand::GoStraight;
            break;
        }
        }
    }
    if (CommandExecuted)
    {
        CommandExecuted = false;
        serial->write('c');
        if (willstopatnxtc)
        {
            state = Stop;
        }
    }
    switch (state) // 根据状态控制电机
    {
    case WorkingState::WStraight:
    case WorkingState::LPending:
    case WorkingState::RPending:
    case WorkingState::StPending:
    {
        LF->Speed(SPEED_STD);
        RF->Speed(SPEED_STD);
        LB->Speed(SPEED_STD);
        RB->Speed(SPEED_STD);
        break;
    }
    case WorkingState::WCorrLeft:
    {
        LF->Speed(SPEED_STD - SPEED_DIFF);
        RF->Speed(SPEED_STD + SPEED_DIFF);
        LB->Speed(SPEED_STD - SPEED_DIFF);
        RB->Speed(SPEED_STD + SPEED_DIFF);
        break;
    }
    case WorkingState::WCorrRight:
    {
        LF->Speed(SPEED_STD + SPEED_DIFF);
        RF->Speed(SPEED_STD - SPEED_DIFF);
        LB->Speed(SPEED_STD + SPEED_DIFF);
        RB->Speed(SPEED_STD - SPEED_DIFF);
        break;
    }
    case WorkingState::TurnLeft:
    {
        LF->Speed(-SPEED_STD);
        RF->Speed(SPEED_STD);
        LB->Speed(-SPEED_STD);
        RB->Speed(SPEED_STD);
        break;
    }
    case WorkingState::TurnRight:
    {
        LF->Speed(SPEED_STD);
        RF->Speed(-SPEED_STD);
        LB->Speed(SPEED_STD);
        RB->Speed(-SPEED_STD);
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
}