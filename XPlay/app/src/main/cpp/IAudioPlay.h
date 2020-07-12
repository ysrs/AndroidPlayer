//
// Created by liululu on 2020/6/13.
//

#ifndef XPLAY_IAUDIOPLAY_H
#define XPLAY_IAUDIOPLAY_H


#include "IObserver.h"
#include "XParameter.h"
#include <list>

class IAudioPlay : public IObserver
{
public:
    virtual bool StartPlay(XParameter out) = 0;
    virtual void Close() = 0;
    virtual void Clear();
    // 获取缓冲数据，如果没有则阻塞
    virtual XData GetData();

    // 缓冲满后阻塞
    virtual void Update(XData data) override;

    // 最大缓冲
    int max_frame_ = 100;
    int pts_ = 0;

protected:
    std::list<XData> frames_;
    std::mutex frame_mutex_;
};


#endif //XPLAY_IAUDIOPLAY_H
