//
// Created by liululu on 2020/6/13.
//

#include "GLVideoView.h"
#include "XTexture.h"
#include "XLog.h"


void GLVideoView::SetRender(void *win)
{
    view_ = win;
}

void GLVideoView::Render(XData data)
{
    if (!view_)
    {
        return;
    }

    if (!texture_)
    {
        texture_ = XTexture::Create();
        texture_->Init(view_, (XTextureType)data.format_);
    }
    texture_->Draw(data.datas_, data.width_, data.height_);
}

void GLVideoView::Close()
{
    mux_.lock();

    if (texture_)
    {
        texture_->Drop();
        texture_ = nullptr;
    }
    mux_.unlock();
}


