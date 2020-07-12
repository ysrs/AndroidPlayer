//
// Created by liululu on 2020/6/12.
//

#include "IObserver.h"
#include "XLog.h"


void IObserver::Update(XData data)
{

}

void IObserver::AddObserver(IObserver *observer)
{
    if (observer)
    {
        mux_.lock();
        observers_.push_back(observer);
        mux_.unlock();
    }
}

void IObserver::Notify(XData data)
{
    mux_.lock();
//    XLOGI("IObserver::Notify(XData data) observers_.size():%d, data.is_audio_:%d, data.width_:%d, data.height_:%d", observers_.size(), data.is_audio_, data.width_, data.height_);
    for (int i = 0; i < observers_.size(); ++i)
    {
        observers_[i]->Update(data);
    }

    mux_.unlock();
}

