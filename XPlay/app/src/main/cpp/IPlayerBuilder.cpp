//
// Created by liululu on 2020/6/13.
//

#include "IPlayerBuilder.h"
#include "IVideoView.h"
#include "IResample.h"
#include "IDecode.h"
#include "IAudioPlay.h"
#include "IDemux.h"


IPlayer *IPlayerBuilder::BuilderPlayer(unsigned char index)
{
    IPlayer *play = CreatePlayer(index);

    // 解封装
    IDemux *demux = CreateDemux();

    // 视频解码
    IDecode *video_decode = CreateDecode();

    // 音频解码
    IDecode *audio_decode = CreateDecode();

    // 解码器观察解封装
    video_decode->is_audio_ = false;
    demux->AddObserver(video_decode);
    audio_decode->is_audio_ = true;
    demux->AddObserver(audio_decode);

    //显示观察视频解码器
    IVideoView *video_view = CreateVideoView();
    video_decode->AddObserver(video_view);

    //重采样观察音频解码器
    IResample *resample = CreateResample();
    audio_decode->AddObserver(resample);

    //音频播放观察重采样
    IAudioPlay *audio_play = CreateAudioPlay();
    resample->AddObserver(audio_play);

    play->demux_ = demux;
    play->video_decode_ = video_decode;
    play->audio_decode_ = audio_decode;
    play->video_view_ = video_view;
    play->resample_ = resample;
    play->audio_play_ = audio_play;

    return play;
}

