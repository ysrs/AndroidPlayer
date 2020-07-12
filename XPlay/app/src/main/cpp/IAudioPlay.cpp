//
// Created by liululu on 2020/6/13.
//

#include "IAudioPlay.h"
#include "XLog.h"


void IAudioPlay::Clear()
{
    frame_mutex_.lock();

    while (!frames_.empty())
    {
        frames_.front().Drop();
        frames_.pop_front();
    }

    frame_mutex_.unlock();
}

XData IAudioPlay::GetData()
{
    XData d;

    is_running_ = true;
    while (!is_exit_)
    {
        if (IsPause())
        {
            XSleep(2);
            continue;
        }

        frame_mutex_.lock();
        if (!frames_.empty())
        {
            // 有数据返回
            d = frames_.front();
            frames_.pop_front();
            frame_mutex_.unlock();
            pts_ = d.pts_;
            return d;
        }
        frame_mutex_.unlock();
        XSleep(1);
    }
    is_running_ = false;

    // 未获取数据
    return d;
}

void IAudioPlay::Update(XData data)
{
//    XLOGI("IAudioPlay::Update() data.pts_:%d", data.pts_);
    // 压入缓冲队列
    if (data.size_ <= 0 || !data.data_)
    {
        return;
    }

    while (!is_exit_)
    {
        frame_mutex_.lock();
        if (frames_.size() > max_frame_)
        {
            frame_mutex_.unlock();
            XSleep(1);
            continue;
        }

        frames_.push_back(data);
        frame_mutex_.unlock();
        break;
    }
}

