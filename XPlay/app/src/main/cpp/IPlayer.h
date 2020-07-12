//
// Created by liululu on 2020/6/13.
//

#ifndef XPLAY_IPLAYER_H
#define XPLAY_IPLAYER_H


#include "XThread.h"
#include "XParameter.h"
#include <mutex>

class IDemux;
class IAudioPlay;
class IVideoView;
class IResample;
class IDecode;
class IPlayer : public XThread
{
public:
    static IPlayer *Get(unsigned char index = 0);
    virtual bool Open(const char *path);
    virtual void Close();
    //获取当前的播放进度 0.0 ~ 1.0
    virtual double PlayPos();
    virtual bool Seek(double pos);
    virtual void InitView(void *win);

    bool Start() override;
    void SetPause(bool is_pause) override;

    //是否视频硬解码
    bool is_hard_decode_ = false;

    //音频输出参数配置
    XParameter audio_parameter_;

    IDemux *demux_ = nullptr;
    IDecode *video_decode_ = nullptr;
    IDecode *audio_decode_ = nullptr;
    IResample *resample_ = nullptr;
    IVideoView *video_view_ = nullptr;
    IAudioPlay *audio_play_ = nullptr;

protected:
    IPlayer() {};
    //用作音视频同步
    void Main() override;

    std::mutex mux_;
};


#endif //XPLAY_IPLAYER_H
