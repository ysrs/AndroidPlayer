//
// Created by liululu on 2020/6/13.
//

#ifndef XPLAY_IRESAMPLE_H
#define XPLAY_IRESAMPLE_H


#include "IObserver.h"
#include "XParameter.h"


class IResample : public IObserver
{
public:
    virtual bool Open(XParameter in, XParameter out = XParameter()) = 0;
    virtual XData Resample(XData in_data) = 0;
    virtual void Close() = 0;

    void Update(XData data) override;

    int out_channels_ = 2;
    int out_format_ = 1;
};


#endif //XPLAY_IRESAMPLE_H
