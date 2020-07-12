//
// Created by liululu on 2020/6/12.
//

#ifndef XPLAY_XPARAMETER_H
#define XPLAY_XPARAMETER_H


struct AVCodecParameters;
class XParameter
{
public:
    AVCodecParameters *av_codec_parameters_ = nullptr;
    int channels_ = 2;
    int sample_rate_ = 44100;
};


#endif //XPLAY_XPARAMETER_H
