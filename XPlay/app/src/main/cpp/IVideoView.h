//
// Created by liululu on 2020/6/13.
//

#ifndef XPLAY_IVIDEOVIEW_H
#define XPLAY_IVIDEOVIEW_H


#include "IObserver.h"

class IVideoView : public IObserver
{
public:
    virtual void SetRender(void *win) = 0;
    virtual void Render(XData data) = 0;
    virtual void Close() = 0;

    void Update(XData data) override;
};


#endif //XPLAY_IVIDEOVIEW_H
