//
// Created by liululu on 2020/6/13.
//

#ifndef XPLAY_XEGL_H
#define XPLAY_XEGL_H


class XEGL
{
public:
    static XEGL *Get();
    virtual bool Init(void *win) = 0;
    virtual void Close() = 0;
    virtual void Draw() = 0;

protected:
    XEGL(){}
};


#endif //XPLAY_XEGL_H
