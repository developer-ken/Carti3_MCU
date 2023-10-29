#ifndef _H_APP_LINETRACKING_
#define _H_APP_LINETRACKING_
#include "linetracking_sensor.h"
#include "motor.h"

enum CrossingCommand
{
    Left,
    Right,
    GoStraight
};

enum WorkingState{
    WStraight,WCorrLeft,WCorrRight,
    LPending,RPending,TurnLeft,TurnRight,
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
    void Update(HardwareSerial *);

private:
    ILineTrackingSensor *sensor;
    IMotor *LF, *RF, *LB, *RB;
    void lineTrackingSensorCallback(LineTrackingResult);
    WorkingState state;
    //LineTrackingResult lastresult;
};
#endif