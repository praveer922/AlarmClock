#include <nana/gui.hpp>
#include <nana/gui/widgets/button.hpp>
#include <nana/gui/widgets/label.hpp>
#include <nana/gui/widgets/checkbox.hpp>
#include <iostream>
#include <windows.h>    //used for PlaySound function
#include <mmdeviceapi.h>
#include <endpointvolume.h> 
#include <Functiondiscoverykeys_devpkey.h>
#include <propvarutil.h>
#include <chrono>

using namespace std;

int hour;
int minute;

void play()
{
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

    bool played = PlaySound(TEXT("ride.wav"), NULL, SND_ASYNC | SND_SYSTEM);

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
            if ( secs.count() >= 1) {
                endpointVolume->SetMasterVolumeLevelScalar((float)(currentVolume+0.006), NULL);
                start = current_time;

            }
        }
    }
    endpointVolume->Release();
    CoUninitialize();
}

void waitTillAlarm(const nana::arg_click&) {
    printf("Waiting till %i%i\n", hour,minute);

    while (true) {
        std::chrono::system_clock::time_point now = std::chrono::system_clock::now();
        std::time_t now_c = std::chrono::system_clock::to_time_t(now);
        std::tm now_tm = *std::localtime(&now_c);
        if (hour==now_tm.tm_hour && minute==now_tm.tm_min) {
            play();
            return;
        }
    }

}


int main()
{
    using namespace nana;

    form fm = form(API::make_center(450, 120), {});;
    fm.caption("Alarm Clock");



    // hour input
    label lb_hr(fm, nana::rectangle(55, 45, 30, 30));
    lb_hr.caption("hour:");
    place plc{ fm }; //fm is an instance of nana::form
    plc.div("<hr_radio margin=10>");
    plc["hr_radio"] << lb_hr;

    std::vector<std::unique_ptr<checkbox>> options;
    radio_group rd_group;
    for (int i = 7; i < 12; ++i)
    {
        options.emplace_back(new checkbox{ fm.handle() });
        options.back()->caption(std::to_string(i));
        plc["hr_radio"] << *options.back();

        rd_group.add(*options.back());
    }
    plc.collocate();

    //minute input
    label lb_min(fm, nana::rectangle(120, 45, 30, 30));
    lb_min.caption("min: ");
    plc["hr_radio"] << lb_min;

    std::vector<std::unique_ptr<checkbox>> options_min;
    radio_group rd_group_min;
    for (int i = 0; i < 46; i+=15)
    {
        options_min.emplace_back(new checkbox{ fm.handle() });
        options_min.back()->caption(std::to_string(i));
        plc["hr_radio"] << *options_min.back();

        rd_group_min.add(*options_min.back());
    }

    rd_group.on_clicked([&] {
        hour = rd_group.checked()+7;
        printf("Hour is now: %i\n", hour);
        });

    rd_group_min.on_clicked([&] {
        minute = rd_group_min.checked()*15;
        printf("Min is now: %i\n", minute);
        });






    button btn(fm, nana::rectangle(150, 50, 150, 30));
    btn.caption("Confirm");
    btn.events().click(waitTillAlarm);


    fm.collocate();
    fm.show();
    exec();
}