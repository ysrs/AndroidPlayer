//
// Created by liululu on 2020/6/13.
//

#ifndef XPLAY_XSHADER_H
#define XPLAY_XSHADER_H

#include <mutex>

enum XShaderType
{
    XSHADER_YUV420P = 0,    //软解码和虚拟机
    XSHADER_NV12 = 25,      //手机
    XSHADER_NV21 = 26
};

class XShader
{
public:
    virtual bool Init(XShaderType type = XSHADER_YUV420P);
    //获取材质并映射到内存
    virtual void GetTexture(unsigned int index, int width, int height, unsigned char *buf, bool is_alpha = false);
    virtual void Draw();
    virtual void Close();

protected:
    unsigned int vsh_ = 0;
    unsigned int fsh_ = 0;
    unsigned int program_ = 0;
    unsigned int texts_[100] = {0};
    std::mutex mux_;
};


#endif //XPLAY_XSHADER_H
