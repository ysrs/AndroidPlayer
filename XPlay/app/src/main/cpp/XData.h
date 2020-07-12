//
// Created by liululu on 2020/6/11.
//

#ifndef XPLAY_XDATA_H
#define XPLAY_XDATA_H


enum XDataType
{
    AVPACKET_TYPE = 0,
    UCHAR_TYPE = 1
};


struct XData
{
    int type_ = 0;
    int pts_ = 0;
    unsigned char *data_ = 0;
    unsigned char *datas_[8] = {0};
    int size_ = 0;
    bool is_audio_ = false;
    int width_ = 0;
    int height_ = 0;
    int format_ = 0;
    bool Alloc(int size, const char *data = nullptr);
    void Drop();
};


#endif //XPLAY_XDATA_H
