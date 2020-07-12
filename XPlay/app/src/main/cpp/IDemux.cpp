//
// Created by liululu on 2020/6/12.
//

#include "IDemux.h"
#include "XLog.h"


void IDemux::Main()
{
    while (!is_exit_)
    {
        if (IsPause())
        {
            XSleep(2);
            continue;
        }

        XData d = Read();
        if (d.size_ > 0)
        {
            Notify(d);
        }
        else
        {
            XSleep(2);
        }
    }
}

