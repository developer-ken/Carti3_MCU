#ifndef _H_LINETRACKINGSENSOR_COMMON_INTERFACE_
#define _H_LINETRACKINGSENSOR_COMMON_INTERFACE_
enum LineTrackingResult
{
    Crossing,Loss,
    CorrLeft,CorrRight,Straight
};

class ILineTrackingSensor
{
    public:
    enum LineTrackingResult result;
    bool isMiddleSensorTriggered;
    /// @brief 事件，当传感器状态发生变化时触发。可能会在IRC调用，避免耗时操作
    std::function<void(LineTrackingResult)> LineTrackingStateChangedEvent;
};
#endif