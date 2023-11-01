#include <Arduino.h>
#include "linetracking_5way.h"
LinetrackingSensor5W::LinetrackingSensor5W(PinName LL, PinName L, PinName M, PinName R, PinName RR, int triggerLevel, bool useInterrupt)
{
    this->triggerLevel = triggerLevel;
    pinMode(LL, INPUT);
    pinMode(L, INPUT);
    pinMode(M, INPUT);
    pinMode(R, INPUT);
    pinMode(RR, INPUT);
    this->ll = LL;
    this->l = L;
    this->m = M;
    this->r = R;
    this->rr = RR;
    if (useInterrupt)
    {
        attachInterrupt(digitalPinToInterrupt(L), std::bind(&LinetrackingSensor5W::irc_updateall, this), CHANGE);
        attachInterrupt(digitalPinToInterrupt(LL), std::bind(&LinetrackingSensor5W::irc_updateall, this), CHANGE);
        attachInterrupt(digitalPinToInterrupt(M), std::bind(&LinetrackingSensor5W::irc_updateall, this), CHANGE);
        attachInterrupt(digitalPinToInterrupt(R), std::bind(&LinetrackingSensor5W::irc_updateall, this), CHANGE);
        attachInterrupt(digitalPinToInterrupt(RR), std::bind(&LinetrackingSensor5W::irc_updateall, this), CHANGE);
    }
}

void LinetrackingSensor5W::TriggerOverallUpdate()
{
    this->LL = digitalRead(this->ll) == this->triggerLevel;
    this->L = digitalRead(this->l) == this->triggerLevel;
    this->M = digitalRead(this->m) == this->triggerLevel;
    this->R = digitalRead(this->r) == this->triggerLevel;
    this->RR = digitalRead(this->rr) == this->triggerLevel;
    this->isMiddleSensorTriggered = this->M;
    auto newstate = Loss;
    static int ltt = 0;
    if ((millis() - ltt > 500) && (this->LL || this->RR) && (this->M || this->L || this->R))
    {
        // 外侧传感器接线，路口
        newstate = Crossing;
        ltt = millis();
    }
    else if (this->L && this->R)
    {
        // 左右外侧无黑线，中间左右两传感器黑线，直行
        // 黑线太粗/传感器安装高度错误/阈值不合理，装配要保证尽量不触发这个分支
        newstate = Straight;
    }
    else if (this->L)
    {
        // 内左传感器接线，向左修正
        newstate = CorrLeft;
    }
    else if (this->R)
    {
        // 内右传感器接线，向右修正
        newstate = CorrRight;
    }
    else if (this->M)
    {
        // 仅中间传感器接线，直行
        newstate = Straight;
    }
    if (newstate != result)
    {
        result = newstate;
        if (LineTrackingStateChangedEvent != nullptr)
        {
            LineTrackingStateChangedEvent(result);
        }
    }
}

void LinetrackingSensor5W::irc_updateall()
{
    TriggerOverallUpdate();
}