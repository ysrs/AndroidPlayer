//
// Created by liululu on 2020/6/13.
//

#ifndef XPLAY_FFPLAYERBUILDER_H
#define XPLAY_FFPLAYERBUILDER_H


#include "IPlayerBuilder.h"

class FFPlayerBuilder : public IPlayerBuilder
{
public:
    static void InitHard(void *vm);
    static FFPlayerBuilder *Get();

protected:
    FFPlayerBuilder() {};
    IDemux *CreateDemux() override;
    IDecode *CreateDecode() override;
    IResample *CreateResample() override;
    IVideoView *CreateVideoView() override;
    IAudioPlay *CreateAudioPlay() override;
    IPlayer *CreatePlayer(unsigned char index = 0) override;
};


#endif //XPLAY_FFPLAYERBUILDER_H
