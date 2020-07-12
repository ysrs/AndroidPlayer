//
// Created by liululu on 2020/6/13.
//

#ifndef XPLAY_FFDEMUX_H
#define XPLAY_FFDEMUX_H


#include "IDemux.h"
#include <mutex>


struct AVFormatContext;
class FFDemux : public IDemux
{
public:
    FFDemux();

    bool Open(const char *url) override;
    bool Seek(double pos) override;
    void Close() override;

    XParameter GetVPara() override;
    XParameter GetAPara() override;
    XData Read() override;

private:
    AVFormatContext *ic_ = nullptr;
    std::mutex mux_;
    int video_stream_ = 0;
    int audio_stream_ = 1;
};


#endif //XPLAY_FFDEMUX_H
