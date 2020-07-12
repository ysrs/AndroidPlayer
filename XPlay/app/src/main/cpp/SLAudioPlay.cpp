//
// Created by liululu on 2020/6/13.
//

#include "SLAudioPlay.h"
#include "XLog.h"
#include <SLES/OpenSLES.h>
#include <SLES/OpenSLES_Android.h>


static SLObjectItf engineSL = nullptr;
static SLEngineItf eng = nullptr;
static SLObjectItf mix = nullptr;
static SLObjectItf player = nullptr;
static SLPlayItf iplayer = nullptr;
static SLAndroidSimpleBufferQueueItf pcmQue = nullptr;

static SLEngineItf CreateSL()
{
    SLresult re;
    SLEngineItf en;
    re = slCreateEngine(&engineSL, 0, 0, 0, 0, 0);
    if(re != SL_RESULT_SUCCESS)
    {
        return nullptr;
    }

    re = (*engineSL)->Realize(engineSL, SL_BOOLEAN_FALSE);
    if (re != SL_RESULT_SUCCESS)
    {
        return nullptr;
    }

    re = (*engineSL)->GetInterface(engineSL, SL_IID_ENGINE, &en);
    if(re != SL_RESULT_SUCCESS)
    {
        return nullptr;
    }

    return en;
}

static void PcmCall(SLAndroidSimpleBufferQueueItf bf, void *contex)
{
    SLAudioPlay *ap = (SLAudioPlay *)contex;
    if(!ap)
    {
        XLOGE("PcmCall failed contex is null!");
        return;
    }
    ap->PlayCall((void *)bf);
}

SLAudioPlay::SLAudioPlay()
{
    buf_ = new unsigned char[1024 * 1024];
}

SLAudioPlay::~SLAudioPlay()
{
    delete[] buf_;
    buf_ = nullptr;
}

bool SLAudioPlay::StartPlay(XParameter out)
{
    Close();

    mux_.lock();
    //1 创建引擎
    eng = CreateSL();
    if(eng)
    {
        XLOGI("SLAudioPlay::StartPlay() CreateSL success!");
    }
    else
    {
        mux_.unlock();
        XLOGE("SLAudioPlay::StartPlay() CreateSL failed!");
        return false;
    }

    //2 创建混音器
    SLresult re = 0;
    re = (*eng)->CreateOutputMix(eng, &mix, 0, 0, 0);
    if(re != SL_RESULT_SUCCESS )
    {
        mux_.unlock();
        XLOGE("CreateOutputMix() failed!");
        return false;
    }
    re = (*mix)->Realize(mix, SL_BOOLEAN_FALSE);
    if(re != SL_RESULT_SUCCESS)
    {
        mux_.unlock();
        XLOGE("(*mix)->Realize failed!");
        return false;
    }
    SLDataLocator_OutputMix outmix = {SL_DATALOCATOR_OUTPUTMIX, mix};
    SLDataSink audioSink= {&outmix, 0};

    //3 配置音频信息
    //缓冲队列
    SLDataLocator_AndroidSimpleBufferQueue que = {SL_DATALOCATOR_ANDROIDSIMPLEBUFFERQUEUE, 10};
    //音频格式
    SLDataFormat_PCM pcm = {
            SL_DATAFORMAT_PCM,
            (SLuint32) out.channels_,//    声道数
            (SLuint32) out.sample_rate_ * 1000,
            SL_PCMSAMPLEFORMAT_FIXED_16,
            SL_PCMSAMPLEFORMAT_FIXED_16,
            SL_SPEAKER_FRONT_LEFT|SL_SPEAKER_FRONT_RIGHT,
            SL_BYTEORDER_LITTLEENDIAN //字节序，小端
    };
    SLDataSource ds = {&que, &pcm};

    //4 创建播放器
    const SLInterfaceID ids[] = {SL_IID_BUFFERQUEUE};
    const SLboolean req[] = {SL_BOOLEAN_TRUE};
    re = (*eng)->CreateAudioPlayer(eng, &player, &ds, &audioSink, sizeof(ids)/sizeof(SLInterfaceID), ids, req);
    if(re !=SL_RESULT_SUCCESS )
    {
        mux_.unlock();
        XLOGE("CreateAudioPlayer failed!");
        return false;
    }
    else
    {
        XLOGI("CreateAudioPlayer success!");
    }
    (*player)->Realize(player, SL_BOOLEAN_FALSE);
    //获取player接口
    re = (*player)->GetInterface(player, SL_IID_PLAY, &iplayer);
    if(re != SL_RESULT_SUCCESS)
    {
        mux_.unlock();
        XLOGE("GetInterface SL_IID_PLAY failed!");
        return false;
    }
    re = (*player)->GetInterface(player, SL_IID_BUFFERQUEUE, &pcmQue);
    if(re != SL_RESULT_SUCCESS)
    {
        mux_.unlock();
        XLOGE("GetInterface SL_IID_BUFFERQUEUE failed!");
        return false;
    }

    //设置回调函数，播放队列空调用
    (*pcmQue)->RegisterCallback(pcmQue, PcmCall, this);

    //设置为播放状态
    (*iplayer)->SetPlayState(iplayer, SL_PLAYSTATE_PLAYING);

    //启动队列回调
    (*pcmQue)->Enqueue(pcmQue, "", 1);
    is_exit_ = false;
    mux_.unlock();
    XLOGI("SLAudioPlay::StartPlay success!");
    return true;
}

void SLAudioPlay::Close()
{
    IAudioPlay::Clear();

    mux_.lock();
    //停止播放
    if(iplayer && (*iplayer))
    {
        (*iplayer)->SetPlayState(iplayer, SL_PLAYSTATE_STOPPED);
    }
    //清理播放队列
    if(pcmQue && (*pcmQue))
    {
        (*pcmQue)->Clear(pcmQue);
    }
    //销毁player对象
    if(player && (*player))
    {
        (*player)->Destroy(player);
    }
    //销毁混音器
    if(mix && (*mix))
    {
        (*mix)->Destroy(mix);
    }

    //销毁播放引擎
    if(engineSL && (*engineSL))
    {
        (*engineSL)->Destroy(engineSL);
    }

    engineSL = NULL;
    eng = NULL;
    mix = NULL;
    player = NULL;
    iplayer = NULL;
    pcmQue = NULL;
    mux_.unlock();
}

void SLAudioPlay::PlayCall(void *buf)
{
    if(!buf)
    {
        return;
    }

    SLAndroidSimpleBufferQueueItf bf = (SLAndroidSimpleBufferQueueItf)buf;
    //XLOGE("SLAudioPlay::PlayCall");
    //阻塞
    XData d = GetData();
    if(d.size_ <= 0)
    {
        XLOGE("GetData() size is 0");
        return;
    }
    if(!buf_)
    {
        return;
    }
    memcpy(buf_, d.data_, d.size_);

    mux_.lock();
    if(pcmQue && (*pcmQue))
    {
        (*pcmQue)->Enqueue(pcmQue, buf_, d.size_);
    }
    mux_.unlock();

    d.Drop();
}
