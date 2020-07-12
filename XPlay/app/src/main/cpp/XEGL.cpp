//
// Created by liululu on 2020/6/13.
//

#include "XEGL.h"
#include "XLog.h"
#include <mutex>
#include <android/native_window.h>
#include <EGL/egl.h>


class CXEGL : public XEGL
{
public:
    void Draw() override
    {
        mux_.lock();
        if(display_ == EGL_NO_DISPLAY || surface_ == EGL_NO_SURFACE)
        {
            mux_.unlock();
            return;
        }
        eglSwapBuffers(display_, surface_);
        mux_.unlock();
    }

    void Close() override
    {
        mux_.lock();
        if(display_ == EGL_NO_DISPLAY)
        {
            mux_.unlock();
            return;
        }
        eglMakeCurrent(display_, EGL_NO_SURFACE, EGL_NO_SURFACE, EGL_NO_CONTEXT);

        if(surface_ != EGL_NO_SURFACE)
        {
            eglDestroySurface(display_, surface_);
        }
        if(context_ != EGL_NO_CONTEXT)
        {
            eglDestroyContext(display_, context_);
        }

        eglTerminate(display_);

        display_ = EGL_NO_DISPLAY;
        surface_ = EGL_NO_SURFACE;
        context_ = EGL_NO_CONTEXT;
        mux_.unlock();
    }

    bool Init(void *win) override
    {
        ANativeWindow *native_window = (ANativeWindow *)win;
        Close();

        //初始化EGL
        mux_.lock();
        //1 获取EGLDisplay对象 显示设备
        display_ = eglGetDisplay(EGL_DEFAULT_DISPLAY);
        if(display_ == EGL_NO_DISPLAY)
        {
            mux_.unlock();
            XLOGE("eglGetDisplay failed!");
            return false;
        }
        XLOGI("eglGetDisplay success!");
        //2 初始化Display
        if(EGL_TRUE != eglInitialize(display_, 0, 0))
        {
            mux_.unlock();
            XLOGE("eglInitialize failed!");
            return false;
        }
        XLOGI("eglInitialize success!");

        //3 获取配置并创建surface
        EGLint configSpec [] =
        {
            EGL_RED_SIZE,8,
            EGL_GREEN_SIZE,8,
            EGL_BLUE_SIZE,8,
            EGL_SURFACE_TYPE,EGL_WINDOW_BIT,
            EGL_NONE
        };
        EGLConfig config = 0;
        EGLint numConfigs = 0;
        if(EGL_TRUE != eglChooseConfig(display_, configSpec, &config, 1, &numConfigs))
        {
            mux_.unlock();
            XLOGE("eglChooseConfig failed!");
            return false;
        }
        XLOGI("eglChooseConfig success!");
        surface_ = eglCreateWindowSurface(display_, config, native_window, NULL);

        //4 创建并打开EGL上下文
        const EGLint ctxAttr[] = { EGL_CONTEXT_CLIENT_VERSION, 2, EGL_NONE};
        context_ = eglCreateContext(display_, config, EGL_NO_CONTEXT, ctxAttr);
        if(context_ == EGL_NO_CONTEXT)
        {
            mux_.unlock();
            XLOGE("eglCreateContext failed!");
            return false;
        }
        XLOGI("CXEGL Init() eglCreateContext success!");

        if(EGL_TRUE != eglMakeCurrent(display_, surface_, surface_, context_))
        {
            mux_.unlock();
            XLOGE("eglMakeCurrent failed!");
            return false;
        }
        XLOGI("eglMakeCurrent success!");
        mux_.unlock();
        return true;
    }

private:
    EGLDisplay display_ = EGL_NO_DISPLAY;
    EGLSurface surface_ = EGL_NO_SURFACE;
    EGLContext context_ = EGL_NO_CONTEXT;
    std::mutex mux_;
};


XEGL *XEGL::Get()
{
    static CXEGL egl;
    return &egl;
}
