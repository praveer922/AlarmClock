#ifndef ALARMCLOCK_H
#define ALARMCLOCK_H
#include <string>

class AlarmClock
{
private:
    int m_hour = 0;
    int m_minute = 0;
    std::string alarm_track;

public:
    AlarmClock();

    void SetHour(int hour);
    void SetMinute(int minute);
    void SetSound(std::string &alarm_track);
    void WaitTillAlarm();
    void PlayAlarm();
};

#endif
