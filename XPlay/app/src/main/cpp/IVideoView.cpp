//
// Created by liululu on 2020/6/13.
//

#include "IVideoView.h"
#include "XLog.h"


void IVideoView::Update(XData data)
{
    //XLOGI("IVideoView::Update() data.is_audio_:%d, data.pts_:%d, data.width_:%d, data.height_:%d", data.is_audio_, data.pts_, data.width_, data.height_);
    Render(data);
}

