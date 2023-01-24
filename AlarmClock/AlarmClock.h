#ifndef ALARMCLOCK_H
#define ALARMCLOCK_H

class AlarmClock
{
private:
    int m_hour = 0;
    int m_minute = 0;
    const char * alarm_track;

public:
    AlarmClock();

    void SetHour(int hour);
    void SetMinute(int minute);
    void SetSound(const char* alarm_track);
    void WaitTillAlarm();
    void PlayAlarm();
};

#endif
