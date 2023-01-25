#include "AlarmClock.h"
#include <windows.h>    //used for PlaySound function
#include <mmdeviceapi.h>
#include <endpointvolume.h> 
#include <Functiondiscoverykeys_devpkey.h>
#include <propvarutil.h>
#include <chrono>
#include <time.h>
#include <string>
#include <thread>

AlarmClock::AlarmClock() = default;

void AlarmClock::SetHour(int hour) {
    m_hour = hour;
}

void AlarmClock::SetMinute(int minute) {
    m_minute = minute;
}

void AlarmClock::SetSound(std::string &alarmtrack) {
    alarm_track = alarmtrack;
}

void AlarmClock::Wait() {
    while (true) {
        std::chrono::system_clock::time_point now = std::chrono::system_clock::now();
        std::time_t now_c = std::chrono::system_clock::to_time_t(now);
        struct tm now_tm;
        localtime_s(&now_tm, &now_c);
        if (m_hour == now_tm.tm_hour && m_minute == now_tm.tm_min) {
            PlayAlarm();
            return;
        }
    }
}

void AlarmClock::WaitTillAlarm() {
    printf("Waiting till %i%i\n", m_hour, m_minute);
    std::thread t = std::thread(&AlarmClock::Wait, this);
    t.detach();
}

void AlarmClock::Play() {
    HRESULT hr;

    CoInitialize(NULL);
    IMMDeviceEnumerator* deviceEnumerator = NULL;
    hr = CoCreateInstance(__uuidof(MMDeviceEnumerator), NULL, CLSCTX_INPROC_SERVER, __uuidof(IMMDeviceEnumerator), (LPVOID*)&deviceEnumerator);
    IMMDevice* defaultDevice = NULL;
    hr = deviceEnumerator->GetDefaultAudioEndpoint(eRender, eConsole, &defaultDevice);
    deviceEnumerator->Release();
    deviceEnumerator = NULL;

    IAudioEndpointVolume* endpointVolume = NULL;
    hr = defaultDevice->Activate(__uuidof(IAudioEndpointVolume), CLSCTX_INPROC_SERVER, NULL, (LPVOID*)&endpointVolume);
    defaultDevice->Release();
    defaultDevice = NULL;

    float currentVolume = 0;

    hr = endpointVolume->SetMasterVolumeLevelScalar((float)0, NULL);

    std::wstring trackwstr = std::wstring(alarm_track.begin(), alarm_track.end());

    bool played = PlaySound(trackwstr.c_str(), NULL, SND_ASYNC | SND_SYSTEM);

    // every second, increase volume by 0.6, till it reaches a maximum of 65 (that should wake me up)
    auto start = std::chrono::system_clock::now();
    while (true) {
        endpointVolume->GetMasterVolumeLevelScalar(&currentVolume);
        printf("Current volume is now: %f\n", currentVolume);
        if (currentVolume >= 0.65) {
            break;
        }
        else {
            auto current_time = std::chrono::system_clock::now();
            typedef std::chrono::duration<float> float_seconds;
            auto secs = std::chrono::duration_cast<float_seconds>(current_time - start);
            if (secs.count() >= 1) {
                endpointVolume->SetMasterVolumeLevelScalar((float)(currentVolume + 0.006), NULL);
                start = current_time;

            }
        }
    }
    endpointVolume->Release();
    CoUninitialize();

}

void AlarmClock::PlayAlarm() {
    std::thread t = std::thread(&AlarmClock::Play, this);
    t.detach();
}