//
// Created by liululu on 2020/6/13.
//

#ifndef XPLAY_GLVIDEOVIEW_H
#define XPLAY_GLVIDEOVIEW_H


#include "IVideoView.h"
#include "XData.h"

class XTexture;
class GLVideoView : public IVideoView
{
public:
    void SetRender(void *win) override;
    void Render(XData data) override;
    void Close() override;

protected:
    void *view_ = nullptr;
    XTexture *texture_ = nullptr;
    std::mutex mux_;
};


#endif //XPLAY_GLVIDEOVIEW_H
