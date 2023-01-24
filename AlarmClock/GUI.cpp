#include <nana/gui.hpp>
#include <nana/gui/widgets/button.hpp>
#include <nana/gui/widgets/label.hpp>
#include <nana/gui/widgets/checkbox.hpp>
#include <iostream>
#include "AlarmClock.h"

using namespace std;


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

    AlarmClock ac;

    rd_group.on_clicked([&] {
        int hour = rd_group.checked() + 7;
        ac.SetHour(hour);
        printf("Hour is now: %i\n", hour);
        });

    rd_group_min.on_clicked([&] {
        int minute = rd_group_min.checked()*15;
        ac.SetMinute(minute);
        printf("Min is now: %i\n", minute);
        });


    button btn(fm, nana::rectangle(150, 50, 150, 30));
    btn.caption("Confirm");
    btn.events().click([&](const nana::arg_click&) {
        ac.WaitTillAlarm();
        });

    button test_btn(fm, nana::rectangle(150, 90, 150, 30));
    test_btn.caption("Test Sound");
    test_btn.events().click([&](const nana::arg_click&) {
        ac.PlayAlarm();
        });


    fm.collocate();
    fm.show();
    exec();
}