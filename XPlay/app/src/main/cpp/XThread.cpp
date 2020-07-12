//
// Created by liululu on 2020/6/12.
//

#include "XThread.h"
#include "XLog.h"
#include <thread>
#include <sys/time.h>


using namespace std;

void XSleep(int ms)
{
    chrono::milliseconds du(ms);
    this_thread::sleep_for(du);
}

long long GetSystemTime()
{
    struct timeval tv;
    gettimeofday(&tv, nullptr);
    return (tv.tv_sec * 1000 + tv.tv_usec / 1000);
}

bool XThread::Start()
{
    is_exit_ = false;
    is_pause_ = false;
    thread th(&XThread::ThreadMain, this);
    th.detach();

    return true;
}

void XThread::Stop()
{
    XLOGI("Stop 停止线程begin!");

    is_exit_ = true;
    for(int i = 0; i < 200; i++)
    {
        if(!is_running_)
        {
            XLOGI("Stop 停止线程成功!");

            return;
        }
        XSleep(1);
    }

    XLOGI("Stop 停止线程超时!");
}

void XThread::SetPause(bool is_pause)
{
    is_pause_ = is_pause;
    //等待100毫秒
    for(int i = 0; i < 10; i++)
    {
        if(is_pausing_ == is_pause)
        {
            break;
        }
        XSleep(10);
    }
}

bool XThread::IsPause()
{
    is_pausing_ = is_pause_;
    return is_pause_;
}

void XThread::Main()
{
}

void XThread::ThreadMain()
{
    is_running_ = true;
    XLOGI("线程函数进入");
    Main();
    XLOGI("线程函数退出");
    is_running_ = false;
}

