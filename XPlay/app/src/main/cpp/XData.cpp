//
// Created by liululu on 2020/6/11.
//

#include "XData.h"

extern "C"
{
#include <libavformat/avformat.h>
}

bool XData::Alloc(int size, const char *data)
{
    Drop();
    type_ = UCHAR_TYPE;
    if (size <= 0)
    {
        return false;
    }
    data_ = new unsigned char[size];
    if (data)
    {
        memcpy(data_, data, size);
    }
    size_ = size;
    return true;
}

void XData::Drop()
{
    if (!data_)
    {
        return;
    }

    if (type_ == AVPACKET_TYPE)
    {
        av_packet_free((AVPacket **)&data_);
    }
    else
    {
        delete [] data_;
    }

    data_ = nullptr;
    size_ = 0;
}

