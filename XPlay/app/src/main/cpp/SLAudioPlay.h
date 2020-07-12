//
// Created by liululu on 2020/6/13.
//

#ifndef XPLAY_SLAUDIOPLAY_H
#define XPLAY_SLAUDIOPLAY_H


#include "IAudioPlay.h"
#include <mutex>

class SLAudioPlay : public IAudioPlay
{
public:
    SLAudioPlay();
    virtual ~SLAudioPlay();

    bool StartPlay(XParameter out) override ;
    void Close() override;
    void PlayCall(void *buf);

protected:
    unsigned char *buf_ = nullptr;
    std::mutex mux_;
};


#endif //XPLAY_SLAUDIOPLAY_H
