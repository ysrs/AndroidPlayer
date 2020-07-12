//
// Created by liululu on 2020/6/12.
//

#include "IDecode.h"
#include "XLog.h"


void IDecode::Update(XData pkt)
{
    if (pkt.is_audio_ != is_audio_)
    {
        return;
    }
//    XLOGD("IDecode::Update() pkt.is_audio_:%d, pkt.pts_:%d", pkt.is_audio_, pkt.pts_);

    while (!is_exit_)
    {
        packs_mutex_.lock();
        // 阻塞
        if (packs_.size() < max_list_)
        {
            // 生产者
            packs_.push_back(pkt);
            packs_mutex_.unlock();
            break;
        }
        packs_mutex_.unlock();
        XSleep(1);
    }
}

void IDecode::Clear()
{
    packs_mutex_.lock();

    while (!packs_.empty())
    {
        packs_.front().Drop();
        packs_.pop_front();
    }

    pts_ = 0;
    syn_pts_ = 0;

    packs_mutex_.unlock();
}

void IDecode::Main()
{
    long long start = GetSystemTime(), cur = GetSystemTime();
    while (!is_exit_)
    {
        if (IsPause())
        {
            XSleep(2);
            continue;
        }
//        XLOGI("IDecode::Main() is_audio_:%d, syn_pts_:%d", is_audio_, syn_pts_);

        if (!is_audio_)
        {
            cur  = GetSystemTime();
            if (cur - start >= 40)
            {
                start += 40;
            }
            else
            {
                XSleep(1);
                continue;
            }
        }

        packs_mutex_.lock();
        // 判断音视频是否同步
        if (!is_audio_ && syn_pts_ > 0)
        {
//            XLOGD("IDecode::Main() syn_pts_:%d", syn_pts_);
            if (syn_pts_ < pts_)
            {
                packs_mutex_.unlock();
                XSleep(1);
                continue;
            }
        }

        if (packs_.empty())
        {
            packs_mutex_.unlock();
            XSleep(1);
            continue;
        }

        // 取出packet，消费者
        XData pack = packs_.front();
        packs_.pop_front();

        // 发送数据到解码线程，一个数据包，可能包含多个结果
        if (SendPacket(pack))
        {
            while (!is_exit_)
            {
                // 获取解码数据
                XData frame = RecvFrame();
                if (!frame.data_)
                {
                    break;
                }
                pts_ = frame.pts_;

//                XLOGI("IDecode::Main() is_audio_:%d, pts_:%d, %d, %d", frame.is_audio_, frame.pts_, frame.width_, frame.height_);

                // 发送数据给观察者
                Notify(frame);
            }
        }
        pack.Drop();
        packs_mutex_.unlock();
    }
}

