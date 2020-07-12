//
// Created by liululu on 2020/6/13.
//

#include "FFDemux.h"
#include "XLog.h"
extern "C"
{
#include <libavformat/avformat.h>
}


//分数转为浮点数
static double r2d(AVRational r)
{
    return r.num == 0 || r.den == 0 ? 0.0 :(double) r.num / (double)r.den;
}

FFDemux::FFDemux()
{
    static bool is_first = true;
    if (is_first)
    {
        is_first = false;
        // 注册所有封装器
        av_register_all();
        // 注册所有解码器
        avcodec_register_all();
        // 初始化网络
        avformat_network_init();
        XLOGI("register ffmpeg!");
    }
}

bool FFDemux::Open(const char *url)
{
    XLOGI("FFDemux::Open() url:%s", url);
    Close();
    mux_.lock();

    AVDictionary *opts = nullptr;
    // 设置rtsp流以tcp协议打开
    av_dict_set(&opts, "rtsp_transport", "tcp", 0);
    // 设置网络延时
    av_dict_set(&opts, "max_delay", "500", 0);

    int re = avformat_open_input(&ic_, url, nullptr, &opts);
    if (re != 0)
    {
        mux_.unlock();
        char buf[1024] = {0};
        av_strerror(re, buf, sizeof(buf));
        XLOGE("FFDemux:Open() avformat_open_input %s failed! reason:%s", url, buf);
        return false;
    }
    XLOGI("FFDemux:Open() avformat_open_input %s success!", url);

    // 读取文件信息
    re = avformat_find_stream_info(ic_, nullptr);
    if (re != 0)
    {
        mux_.unlock();
        char buf[1024] = {0};
        av_strerror(re, buf, sizeof(buf));
        XLOGE("FFDemux:Open() avformat_find_stream_info %s failed!", url);
        return false;
    }
    XLOGI("FFDemux:Open() avformat_find_stream_info %s success!", url);
    total_time_ = ic_->duration / (AV_TIME_BASE / 1000);

    mux_.unlock();
    XLOGI("FFDemux:Open() total_time_:%d", total_time_);

    GetVPara();
    GetAPara();

    return true;
}

bool FFDemux::Seek(double pos)
{
    if (pos < 0.0 || pos > 1.0)
    {
        XLOGE("FFDemux::Seek pos must in 0.0 ~ 1.0, pos:%f", pos);
        return false;
    }

    mux_.lock();
    if (!ic_)
    {
        mux_.unlock();
        return false;
    }
    // 清理缓冲区的内容
    avformat_flush(ic_);
    long long seek_pos = ic_->streams[video_stream_]->duration * pos;

    // 往后跳转到关键帧
    bool re = av_seek_frame(ic_, video_stream_, seek_pos, AVSEEK_FLAG_FRAME | AVSEEK_FLAG_BACKWARD);
    mux_.unlock();

    return re;
}

void FFDemux::Close()
{
    mux_.lock();
    if (ic_)
    {
        avformat_close_input(&ic_);
    }
    mux_.unlock();
}

XParameter FFDemux::GetVPara()
{
    mux_.lock();
    if (!ic_)
    {
        mux_.unlock();
        XLOGE("FFDemux::GetVPara() faild, ic_ is nullptr!");
        return XParameter();
    }
    // 获取视频流索引
    int re = av_find_best_stream(ic_, AVMEDIA_TYPE_VIDEO, -1, -1, 0, 0);
    if (re < 0)
    {
        mux_.unlock();
        XLOGE("FFDemux::GetVPara() av_find_best_stream failed!");
        return XParameter();
    }
    video_stream_ = re;
//    AVStream *as = ic_->streams[video_stream_];
//    XLOGI("FFDemux::GetVPara() video_stream_:%d, width:%d, height:%d", video_stream_, as->codecpar->width, as->codecpar->height);
    XParameter para;
    para.av_codec_parameters_ = ic_->streams[re]->codecpar;
    mux_.unlock();

    return para;
}

XParameter FFDemux::GetAPara()
{
    mux_.lock();
    if (!ic_)
    {
        mux_.unlock();
        XLOGE("FFDemux::GetAPara() failed! ic_ is nullptr！");
        return XParameter();
    }
    //获取了音频流索引
    int re = av_find_best_stream(ic_, AVMEDIA_TYPE_AUDIO, -1, -1, 0, 0);
    if (re < 0)
    {
        mux_.unlock();
        XLOGE("FFDemux::GetAPara() av_find_best_stream failed!");
        return XParameter();
    }

    audio_stream_ = re;
//    XLOGI("FFDemux::GetAPara() audio_stream_:%d", audio_stream_);
    XParameter para;
    para.av_codec_parameters_ = ic_->streams[re]->codecpar;
    para.channels_ = ic_->streams[re]->codecpar->channels;
    para.sample_rate_ = ic_->streams[re]->codecpar->sample_rate;
    mux_.unlock();

    return para;
}

XData FFDemux::Read()
{
    mux_.lock();
    if(!ic_)
    {
        mux_.unlock();
        return XData();
    }

    XData d;
    AVPacket *pkt = av_packet_alloc();
    int re = av_read_frame(ic_, pkt);
    if(re != 0)
    {
        mux_.unlock();
        av_packet_free(&pkt);
        return XData();
    }
    //XLOGI("FFDemux::Read() pack size is %d ptss %lld", pkt->size, pkt->pts);
    d.data_ = (unsigned char*)pkt;
    d.size_ = pkt->size;
    if(pkt->stream_index == audio_stream_)
    {
        d.is_audio_ = true;
    }
    else if(pkt->stream_index == video_stream_)
    {
        d.is_audio_ = false;
    }
    else
    {
        mux_.unlock();
        av_packet_free(&pkt);
        return XData();
    }

    // 转换pts
    pkt->pts = pkt->pts * (1000 * r2d(ic_->streams[pkt->stream_index]->time_base));
    pkt->dts = pkt->dts * (1000*r2d(ic_->streams[pkt->stream_index]->time_base));
    d.pts_ = (int)pkt->pts;
    mux_.unlock();
//    XLOGI("FFDemux::Read() demux is_audio_:%d pts:%d, dts:%d, size:%d, duration:%d, flag:%d, pos:%d, side_data_elems:%d, stream_index:%d", d.is_audio_, pkt->pts, pkt->dts, pkt->size, pkt->duration, pkt->flags, pkt->pos, pkt->side_data_elems, pkt->stream_index);

    return d;
}

