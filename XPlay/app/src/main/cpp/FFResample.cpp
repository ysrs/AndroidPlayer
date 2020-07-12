//
// Created by liululu on 2020/6/13.
//

#include "FFResample.h"
#include "XLog.h"
extern "C"
{
#include <libswresample/swresample.h>
#include <libavcodec/avcodec.h>
}


bool FFResample::Open(XParameter in,XParameter out)
{
    Close();

    mux_.lock();
    //音频重采样上下文初始化
    swr_context_ = swr_alloc();
    swr_context_ = swr_alloc_set_opts(swr_context_,
                              av_get_default_channel_layout(out.channels_),
                              AV_SAMPLE_FMT_S16, out.sample_rate_,
                              av_get_default_channel_layout(in.av_codec_parameters_->channels),
                              (AVSampleFormat)in.av_codec_parameters_->format, in.av_codec_parameters_->sample_rate,
                              0, 0);

    int re = swr_init(swr_context_);
    if(re != 0)
    {
        mux_.unlock();
        XLOGE("FFResample::Open() swr_init failed!");
        return false;
    }
    else
    {
        XLOGI("FFResample::Open() swr_init success!");
    }
    out_channels_ = in.av_codec_parameters_->channels;
    out_format_ = AV_SAMPLE_FMT_S16;
    mux_.unlock();

    return true;
}

void FFResample::Close()
{
    mux_.lock();
    if(swr_context_)
    {
        swr_free(&swr_context_);
    }
    mux_.unlock();
}

XData FFResample::Resample(XData in_data)
{
    if(in_data.size_ <= 0 || !in_data.data_)
    {
        return XData();
    }

    mux_.lock();
    if(!swr_context_)
    {
        mux_.unlock();
        return XData();
    }

    //XLOGI("FFResample::Resample() in_data pts_ is %d", in_data.pts_);
    AVFrame *frame = (AVFrame *)in_data.data_;

    //输出空间的分配
    XData out;
    int out_size = out_channels_ * frame->nb_samples * av_get_bytes_per_sample((AVSampleFormat)out_format_);
    if(out_size <= 0)
    {
        return XData();
    }
    out.Alloc(out_size);
    uint8_t *out_arr[2] = {0};
    out_arr[0] = out.data_;
    int len = swr_convert(swr_context_, out_arr, frame->nb_samples, (const uint8_t **)frame->data, frame->nb_samples);
    if(len <= 0)
    {
        mux_.unlock();
        out.Drop();
        return XData();
    }
    out.pts_ = in_data.pts_;
    mux_.unlock();
    //XLOGI("FFResample::Resample() swr_convert success = %d", len);
    return out;
}

