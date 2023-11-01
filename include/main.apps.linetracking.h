#ifndef _H_APP_LINETRACKING_
#define _H_APP_LINETRACKING_
#include "linetracking_sensor.h"
#include "motor.h"

enum CrossingCommand
{
    Left,
    Right,
    GoStraight,
    CStop,
    Alarm
};

enum WorkingState{
    WStraight,WCorrLeft,WCorrRight,
    LPending,RPending,TurnLeft,TurnRight,
    LDelay,RDelay,
    StPending,
    Stop,
    NormPending
};

class LineTracking
{
public:
    CrossingCommand CmdAtNextCrossing;
    bool CommandExecuted;
    LineTracking(ILineTrackingSensor *sensor, IMotor *lf, IMotor *rf, IMotor *lb, IMotor *rb);
    int Update(CrossingCommand* cmdQueue, int cmdQueueLength);
    WorkingState state;

private:
    ILineTrackingSensor *sensor;
    IMotor *LF, *RF, *LB, *RB;
    void lineTrackingSensorCallback(LineTrackingResult);
    //LineTrackingResult lastresult;
};
#endif