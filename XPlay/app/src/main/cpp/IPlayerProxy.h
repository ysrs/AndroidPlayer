//
// Created by liululu on 2020/6/13.
//

#ifndef XPLAY_IPLAYPROXY_H
#define XPLAY_IPLAYPROXY_H


#include "IPlayer.h"
#include <mutex>

class IPlayerProxy : public IPlayer
{
public:
    static IPlayerProxy *Get();
    void Init(void *vm = nullptr);

    bool Open(const char *url) override;
    bool Seek(double pos) override;
    void Close() override;
    bool Start() override;
    void InitView(void *win) override;
    void SetPause(bool is_pause) override;
    bool IsPause() override;
    // 获取当前的播放进度 0.0 ~ 1.0
    double PlayPos() override;

protected:
    IPlayerProxy() {}
    IPlayer *player_ = nullptr;
    std::mutex mux_;
};


#endif //XPLAY_IPLAYPROXY_H
