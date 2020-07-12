//
// Created by liululu on 2020/6/13.
//

#include "XTexture.h"
#include "XLog.h"
#include "XEGL.h"
#include "XShader.h"


class CXTexture : public XTexture
{
public:
    void Drop() override
    {
        mux_.lock();
        XEGL::Get()->Close();
        shader_.Close();
        mux_.unlock();
        delete this;
    }

    bool Init(void *win, XTextureType type) override
    {
        mux_.lock();
        XEGL::Get()->Close();
        shader_.Close();
        type_ = type;
        if(!win)
        {
            mux_.unlock();
            XLOGE("XTexture Init failed win is NULL");
            return false;
        }
        if(!XEGL::Get()->Init(win))
        {
            mux_.unlock();
            return false;
        }
        shader_.Init((XShaderType)type);
        mux_.unlock();

        return true;
    }

    void Draw(unsigned char *data[], int width, int height) override
    {
        mux_.lock();
        shader_.GetTexture(0, width, height, data[0]);  // Y
        if(type_ == XTEXTURE_YUV420P)
        {
            shader_.GetTexture(1, width / 2, height/2, data[1]);  // U
            shader_.GetTexture(2, width / 2, height/2, data[2]);  // V
        }
        else
        {
            shader_.GetTexture(1, width / 2, height / 2, data[1], true);  // UV
        }
        shader_.Draw();
        XEGL::Get()->Draw();
        mux_.unlock();
    }

private:
    XShader shader_;
    XTextureType type_;
    std::mutex mux_;
};

XTexture *XTexture::Create()
{
    return  new CXTexture();
}

