//
// Created by liululu on 2020/6/13.
//

#include "IPlayerProxy.h"
#include "FFPlayerBuilder.h"
#include "XLog.h"


IPlayerProxy *IPlayerProxy::Get()
{
    static IPlayerProxy px;
    return &px;
}

void IPlayerProxy::Init(void *vm)
{
    mux_.lock();
    if(vm)
    {
        FFPlayerBuilder::InitHard(vm);
    }
    if(!player_)
    {
        player_ = FFPlayerBuilder::Get()->BuilderPlayer();
    }
    mux_.unlock();
}

bool IPlayerProxy::Open(const char *url)
{
    bool re = false;

    XLOGI("IPlayerProxy::Open() url:%s", url);

    mux_.lock();
    if(player_)
    {
        player_->is_hard_decode_ = is_hard_decode_;
        XLOGI("IPlayerProxy::Open() player_ true");
        re = player_->Open(url);
    }
    else
    {
        XLOGE("IPlayerProxy::Open() player_ false");
    }
    mux_.unlock();

    return re;
}

bool IPlayerProxy::Seek(double pos)
{
    bool re = false;

    mux_.lock();
    if(player_)
    {
        re = player_->Seek(pos);
    }
    mux_.unlock();

    return re;
}

void IPlayerProxy::Close()
{
    mux_.lock();
    if(player_)
    {
        player_->Close();
    }
    mux_.unlock();
}

bool IPlayerProxy::Start()
{
    bool re = false;

    mux_.lock();
    if(player_)
    {
        re = player_->Start();
    }
    mux_.unlock();

    return re;
}

void IPlayerProxy::InitView(void *win)
{
    mux_.lock();
    if(player_)
    {
        player_->InitView(win);
    }
    mux_.unlock();
}

void IPlayerProxy::SetPause(bool is_pause)
{
    mux_.lock();
    if(player_)
    {
        player_->SetPause(is_pause);
    }
    mux_.unlock();
}

bool IPlayerProxy::IsPause()
{
    bool re = false;

    mux_.lock();
    if(player_)
    {
        re = player_->IsPause();
    }
    mux_.unlock();

    return re;
}

// 获取当前的播放进度 0.0 ~ 1.0
double IPlayerProxy::PlayPos()
{
    double pos = 0.0;

    mux_.lock();
    if(player_)
    {
        pos = player_->PlayPos();
    }
    mux_.unlock();

    return pos;
}


