//
// Created by liululu on 2020/6/13.
//

#include "IPlayer.h"
#include "IDemux.h"
#include "IDecode.h"
#include "IAudioPlay.h"
#include "IVideoView.h"
#include "IResample.h"
#include "XLog.h"


IPlayer *IPlayer::Get(unsigned char index)
{
    static IPlayer p[256];
    return &p[index];
}

bool IPlayer::Open(const char *url)
{
    XLOGI("IPlayer::Open url:%s", url);

    Close();
    mux_.lock();
    //解封装
    if(!demux_ || !demux_->Open(url))
    {
        mux_.unlock();
        XLOGE("IPlayer::Open() demux->Open %s failed!", url);
        return false;
    }
    XLOGI("IPlayer::Open() demux->Open %s success!", url);

    //解码 解码可能不需要，如果是解封之后就是原始数据
    if(!video_decode_ || !video_decode_->Open(demux_->GetVPara(), is_hard_decode_))
    {
        XLOGE("IPlayer::Open() vdecode->Open %s failed!", url);
        //return false;
    }
    XLOGI("IPlayer::Open() vdecode->Open %s success!", url);

    if(!audio_decode_ || !audio_decode_->Open(demux_->GetAPara()))
    {
        XLOGE("IPlayer::Open() audio_decode_->Open %s failed!", url);
        //return false;
    }
    XLOGI("IPlayer::Open() audio_decode_->Open %s success!", url);

    //重采样 有可能不需要，解码后或者解封后可能是直接能播放的数据
    //if(outPara.sample_rate <= 0)
    audio_parameter_ = demux_->GetAPara();
    if(!resample_ || !resample_->Open(demux_->GetAPara(), audio_parameter_))
    {
        XLOGE("IPlayer::Open() resample->Open %s failed!", url);
    }
    XLOGI("IPlayer::Open() resample->Open %s success!", url);
    mux_.unlock();

    return true;
}

void IPlayer::Close()
{
    mux_.lock();
    //2 先关闭主体线程，再清理观察者
    //同步线程
    XThread::Stop();
    //解封装
    if(demux_)
    {
        demux_->Stop();
    }
    //解码
    if(video_decode_)
    {
        video_decode_->Stop();
    }
    if(audio_decode_)
    {
        audio_decode_->Stop();
    }
    if(audio_play_)
    {
        audio_play_->Stop();
    }

    //2 清理缓冲队列
    if(video_decode_)
    {
        video_decode_->Clear();
    }
    if(audio_decode_)
    {
        audio_decode_->Clear();
    }
    if(audio_play_)
    {
        audio_play_->Clear();
    }

    //3 清理资源
    if(audio_play_)
    {
        audio_play_->Close();
    }
    if(video_view_)
    {
        video_view_->Close();
    }
    if(video_decode_)
    {
        video_decode_->Close();
    }
    if(audio_decode_)
    {
        audio_decode_->Close();
    }
    if(demux_)
    {
        demux_->Close();
    }
    mux_.unlock();
}

double IPlayer::PlayPos()
{
    double pos = 0.0;
    mux_.lock();

    int total = 0;
    if(demux_)
    {
        total = demux_->total_time_;
    }
    if(total > 0)
    {
        if(video_decode_)
        {
            pos = (double)video_decode_->pts_ / (double)total;
        }
    }
    mux_.unlock();
    return pos;
}

bool IPlayer::Seek(double pos)
{
    bool re = false;
    if(!demux_)
    {
        return false;
    }

    //暂停所有线程
    SetPause(true);
    mux_.lock();
    //清理缓冲
    //2 清理缓冲队列
    if(video_decode_)
    {
        video_decode_->Clear(); //清理缓冲队列，清理ffmpeg的缓冲
    }
    if(audio_decode_)
    {
        audio_decode_->Clear();
    }
    if(audio_play_)
    {
        audio_play_->Clear();
    }

    re = demux_->Seek(pos); //seek跳转到关键帧
    if(!video_decode_)
    {
        mux_.unlock();
        SetPause(false);
        return re;
    }
    //解码到实际需要显示的帧
    int seek_pts = pos * demux_->total_time_;
    while(!is_exit_)
    {
        XData pkt = demux_->Read();
        if(pkt.size_ <= 0)
        {
            break;
        }
        if(pkt.is_audio_)
        {
            if(pkt.pts_ < seek_pts)
            {
                pkt.Drop();
                continue;
            }
            //写入缓冲队列
            demux_->Notify(pkt);
            continue;
        }

        //解码需要显示的帧之前的数据
        video_decode_->SendPacket(pkt);
        pkt.Drop();
        XData data = video_decode_->RecvFrame();
        if(data.size_ <= 0)
        {
            continue;
        }
        if(data.pts_ >= seek_pts)
        {
            //vdecode->Notify(data);
            break;
        }
    }
    mux_.unlock();

    SetPause(false);
    return re;
}

void IPlayer::InitView(void *win)
{
    if(video_view_)
    {
        video_view_->Close();
        video_view_->SetRender(win);
    }
}

bool IPlayer::Start()
{
    mux_.lock();
    if(video_decode_)
    {
        video_decode_->Start();
    }
    if(audio_decode_)
    {
        audio_decode_->Start();
    }
    if(audio_play_)
    {
        audio_play_->StartPlay(audio_parameter_);
    }
    if(!demux_ || !demux_->Start())
    {
        mux_.unlock();
        XLOGE("demux_->Start failed!");
        return false;
    }
    XThread::Start();
    mux_.unlock();

    return true;
}

void IPlayer::SetPause(bool is_pause)
{
    mux_.lock();
    XThread::SetPause(is_pause);
    if(demux_)
    {
        demux_->SetPause(is_pause);
    }
    if(video_decode_)
    {
        video_decode_->SetPause(is_pause);
    }
    if(audio_decode_)
    {
        audio_decode_->SetPause(is_pause);
    }
    if(audio_play_)
    {
        audio_play_->SetPause(is_pause);
    }
    mux_.unlock();
}

void IPlayer::Main()
{
    while (!is_exit_)
    {
        mux_.lock();
        if(!audio_play_|| !video_decode_)
        {
            mux_.unlock();
            XSleep(2);
            continue;
        }

        //同步
        //获取音频的pts 告诉视频
        int apts = audio_play_->pts_;
        int vpts = video_decode_->pts_;
//        XLOGI("IPlayer::Main() apts:%d, vpts:%d", apts, vpts);
        video_decode_->syn_pts_ = apts;
//        XLOGI("IPlayer::Main() video_decode_->is_audio_:%d", video_decode_->is_audio_);

        mux_.unlock();
        XSleep(2);
    }
}

