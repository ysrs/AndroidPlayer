//
// Created by liululu on 2020/6/13.
//

#ifndef XPLAY_FFDECODE_H
#define XPLAY_FFDECODE_H


#include "IDecode.h"
#include <mutex>

struct AVCodecContext;
struct AVFrame;
class FFDecode : public IDecode
{
public:
    static void InitHard(void *vm);

    bool Open(XParameter para,bool is_hard = false) override;
    void Close() override;
    void Clear() override;
    // future模型 发送数据到线程解码
    bool SendPacket(XData pkt) override;

    //从线程中获取解码结果，再次调用会复用上次空间，线程不安全
    XData RecvFrame() override;

protected:
    AVCodecContext *av_codec_context_ = nullptr;
    AVFrame *av_frame_ = nullptr;
    std::mutex mux_;
};


#endif //XPLAY_FFDECODE_H
