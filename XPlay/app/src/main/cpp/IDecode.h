//
// Created by liululu on 2020/6/12.
//

#ifndef XPLAY_IDECODE_H
#define XPLAY_IDECODE_H


#include "IObserver.h"
#include "XParameter.h"
#include <list>

// 解码接口，支持硬解码
class IDecode : public IObserver
{
public:
    // 打开解码器
    virtual bool Open(XParameter para, bool is_hard = false) = 0;
    virtual void Close() = 0;
    // future模型，发送数据到线程解码
    virtual bool SendPacket(XData pkt) = 0;
    // 从线程中获取解码结果，再次调用会复用上次空间，线程不安全
    virtual XData RecvFrame() = 0;

    //由主体notify的数据 阻塞
    void Update(XData pkt) override;

    virtual void Clear();

    bool is_audio_ = false;
    // 最大的队列长度
    int max_list_ = 100;
    // 同步时间，再次打开文件要清理
    int syn_pts_ = 0;
    int pts_ = 0;

protected:
    void Main() override;

    // 读取缓冲
    std::list<XData> packs_;
    std::mutex packs_mutex_;
};


#endif //XPLAY_IDECODE_H
