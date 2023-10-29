#ifndef _H_LINETRACKINGSENSOR_5WAY_
#define _H_LINETRACKINGSENSOR_5WAY_
#include <Arduino.h>
#include "linetracking_sensor.h"
class LinetrackingSensor5W : public ILineTrackingSensor
{
public:
    LinetrackingSensor5W(PinName LL, PinName L, PinName M, PinName R, PinName RR, int triggerLevel);
    
private:
    PinName ll, l, m, r, rr;
    bool LL, L, M, R, RR;
    int triggerLevel;
    void irc_updateall();
};
#endif