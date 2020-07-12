//
// Created by liululu on 2020/6/13.
//

#include "FFDecode.h"
#include "XLog.h"
#include "XParameter.h"

extern "C"
{
#include <libavcodec/avcodec.h>
#include <libavcodec/jni.h>
}


void FFDecode::InitHard(void *vm)
{
    av_jni_set_java_vm(vm, nullptr);
}

bool FFDecode::Open(XParameter para, bool is_hard)
{
    Close();
    if(!para.av_codec_parameters_)
    {
        return false;
    }

    AVCodecParameters *p = para.av_codec_parameters_;
    //1 查找解码器
    AVCodec *cd = avcodec_find_decoder(p->codec_id);;
    if(is_hard)
    {
        if (AV_CODEC_ID_H264 == p->codec_id)
        {
            // 硬解码264
            cd = avcodec_find_decoder_by_name("h264_mediacodec");
            XLOGI("FFDecode::Open() avcodec_find_decoder h264_mediacodec success.");
        }
        else if (AV_CODEC_ID_MPEG4 == p->codec_id)
        {
            // 硬解码mpeg4
            cd = avcodec_find_decoder_by_name("mpeg4_mediacodec");
            XLOGI("FFDecode::Open() avcodec_find_decoder mpeg4_mediacodec success.");
        }
        else if (AV_CODEC_ID_HEVC == p->codec_id)
        {
            // 硬解码265
            cd = avcodec_find_decoder_by_name("hevc_mediacodec");
            XLOGI("FFDecode::Open() avcodec_find_decoder hevc_mediacodec success.");
        }
        else
        {
            XLOGE("FFDecode::Open() avcodec_find_decoder unknown codec_id:%d.", p->codec_id);
        }
    }

    if(!cd)
    {
        XLOGE("FFDecode::Open() avcodec_find_decoder codec_id:%d failed, is_hard:%d, is_audio_:%d", p->codec_id, is_hard, is_audio_);
        return false;
    }
    XLOGI("FFDecode::Open() avcodec_find_decoder success, is_hard：%d, is_audio_:%d, codec_id:%d", is_hard, is_audio_, p->codec_id);

    mux_.lock();
    //2 创建解码上下文，并复制参数
    av_codec_context_ = avcodec_alloc_context3(cd);
    avcodec_parameters_to_context(av_codec_context_, p);

    av_codec_context_->thread_count = 8;
    //3 打开解码器
    int re = avcodec_open2(av_codec_context_, nullptr, nullptr);
    if(re != 0)
    {
        mux_.unlock();
        char buf[1024] = {0};
        av_strerror(re, buf, sizeof(buf) - 1);
        XLOGE("FFDecode::Open() %s", buf);
        return false;
    }

    if(av_codec_context_->codec_type == AVMEDIA_TYPE_VIDEO)
    {
        is_audio_ = false;
    }
    else
    {
        is_audio_ = true;
    }
    mux_.unlock();
    XLOGI("FFDecode::Open() avcodec_open2 success!");

    return true;
}

void FFDecode::Close()
{
    IDecode::Clear();

    mux_.lock();
    pts_ = 0;
    if(av_frame_)
    {
        av_frame_free(&av_frame_);
    }
    if(av_codec_context_)
    {
        avcodec_close(av_codec_context_);
        avcodec_free_context(&av_codec_context_);
    }
    mux_.unlock();
}

void FFDecode::Clear()
{
    IDecode::Clear();
    mux_.lock();
    if(av_codec_context_)
    {
        avcodec_flush_buffers(av_codec_context_);
    }
    mux_.unlock();
}

bool FFDecode::SendPacket(XData pkt)
{
    if(pkt.size_ <= 0 || !pkt.data_)
    {
        return false;
    }

    mux_.lock();
    if(!av_codec_context_)
    {
        mux_.unlock();
        return false;
    }
    int re = avcodec_send_packet(av_codec_context_, (AVPacket*)pkt.data_);
    mux_.unlock();

    if(re != 0)
    {
        return false;
    }

    return true;
}

XData FFDecode::RecvFrame()
{
    mux_.lock();
    if(!av_codec_context_)
    {
        mux_.unlock();
        return XData();
    }

    if(!av_frame_)
    {
        av_frame_ = av_frame_alloc();
    }
    int re = avcodec_receive_frame(av_codec_context_, av_frame_);
    if(re != 0)
    {
        mux_.unlock();
        return XData();
    }

    XData d;
    d.data_ = (unsigned char *)av_frame_;
//    XLOGI("FFDecode::RecvFrame() av_codec_context_->codec_type:%d", av_codec_context_->codec_type);
    if(av_codec_context_->codec_type == AVMEDIA_TYPE_VIDEO)
    {
        d.size_ = (av_frame_->linesize[0] + av_frame_->linesize[1] + av_frame_->linesize[2]) * av_frame_->height;
        d.width_ = av_frame_->width;
        d.height_ = av_frame_->height;
        d.is_audio_ = false;
    }
    else
    {
        //样本字节数 * 单通道样本数 * 通道数
        d.size_ = av_get_bytes_per_sample((AVSampleFormat)av_frame_->format) * av_frame_->nb_samples * 2;
        d.is_audio_ = true;
    }
    d.format_ = av_frame_->format;
    //if(!is_audio_)
    //    XLOGE("FFDecode::RecvFrame() data format is %d", av_frame_->format);
    memcpy(d.datas_, av_frame_->data, sizeof(d.datas_));
    d.pts_ = av_frame_->pts;
    pts_ = d.pts_;
    mux_.unlock();

//    XLOGI("FFDecode::RecvFrame() is_audio_:%d, pts_:%d, width_:%d, height_:%d, size_:%d", d.is_audio_, d.pts_, d.width_, d.height_, d.size_);

    return d;
}

