//
// Created by liululu on 2020/6/13.
//

#include "IResample.h"


void IResample::Update(XData data)
{
    XData d = Resample(data);
    // XLOGI("IResample::Update() data.size_:%d d.size_:%d", data.size_, d.size_);
    if (d.size_ > 0)
    {
        Notify(d);
    }
}
