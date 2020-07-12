//
// Created by liululu on 2020/6/13.
//

#ifndef XPLAY_FFRESAMPLE_H
#define XPLAY_FFRESAMPLE_H


#include "IResample.h"
#include <mutex>

struct SwrContext;
class FFResample : public IResample
{
public:
    bool Open(XParameter in,XParameter out = XParameter()) override;
    void Close() override;
    XData Resample(XData in_data) override;

protected:
    SwrContext *swr_context_ = nullptr;
    std::mutex mux_;
};


#endif //XPLAY_FFRESAMPLE_H
