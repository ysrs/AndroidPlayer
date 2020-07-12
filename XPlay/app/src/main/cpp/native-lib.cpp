#include <jni.h>
#include <string>
#include <android/native_window_jni.h>
#include "XLog.h"
#include "IPlayerProxy.h"


extern "C"
JNIEXPORT
jint JNI_OnLoad(JavaVM *vm,void *res)
{
    IPlayerProxy::Get()->Init(vm);

    /*IPlayerPorxy::Get()->Open("/sdcard/v1080.mp4");
    IPlayerPorxy::Get()->Start();

    IPlayerPorxy::Get()->Open("/sdcard/1080.mp4");
    IPlayerPorxy::Get()->Start();*/

    return JNI_VERSION_1_4;
}

extern "C"
JNIEXPORT void JNICALL
Java_xplay_xplay_XPlay_InitView(JNIEnv *env, jobject instance, jobject surface) {
    ANativeWindow *win = ANativeWindow_fromSurface(env, surface);
    IPlayerProxy::Get()->InitView(win);
}

extern "C"
JNIEXPORT void JNICALL
Java_xplay_xplay_OpenUrl_Open(JNIEnv *env, jobject instance, jstring url_) {
    const char *url = env->GetStringUTFChars(url_, 0);

    XLOGI("Java_xplay_xplay_OpenUrl_Open() url:%s in.", url);

    IPlayerProxy::Get()->Open(url);
    IPlayerProxy::Get()->Start();
    //IPlayerProxy::Get()->Seek(0.5);

    env->ReleaseStringUTFChars(url_, url);
    XLOGI("Java_xplay_xplay_OpenUrl_Open() url:%s out.", url);
}

extern "C"
JNIEXPORT jdouble JNICALL
Java_xplay_xplay_MainActivity_PlayPos(JNIEnv *env, jobject instance) {
    return IPlayerProxy::Get()->PlayPos();
}

extern "C"
JNIEXPORT void JNICALL
Java_xplay_xplay_MainActivity_Seek(JNIEnv *env, jobject instance, jdouble pos) {
    IPlayerProxy::Get()->Seek(pos);
}

extern "C"
JNIEXPORT void JNICALL
Java_xplay_xplay_XPlay_PlayOrPause(JNIEnv *env, jobject instance) {
    IPlayerProxy::Get()->SetPause(!IPlayerProxy::Get()->IsPause());
}

extern "C"
JNIEXPORT jboolean JNICALL
Java_xplay_xplay_MainActivity_IsPause(JNIEnv *env, jclass instance) {
    return IPlayerProxy::Get()->IsPause();
}

extern "C"
JNIEXPORT void JNICALL
Java_xplay_xplay_MainActivity_SetVideoData(JNIEnv *env, jobject instance, jbyteArray bytes, jint size, jlong pts) {
    XLOGI("SetVideoData->size:%d, pts:%lld", size, pts);
}

extern "C"
JNIEXPORT void JNICALL
Java_xplay_xplay_OpenUrl_SetAudioData(JNIEnv *env, jobject instance, jbyteArray bytes, jint size, jlong pts) {
    XLOGI("SetAudioData->size:%d, pts:%lld", size, pts);
}
