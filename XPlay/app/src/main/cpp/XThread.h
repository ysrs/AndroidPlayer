//
// Created by liululu on 2020/6/12.
//

#ifndef XPLAY_XTHREAD_H
#define XPLAY_XTHREAD_H

//sleep 毫秒
void XSleep(int ms);
// 获取系统时间，毫秒
long long GetSystemTime();

//c++ 11 线程库
class XThread
{
public:
//启动线程
    virtual bool Start();

    //通过控制is_exit_安全停止线程（不一定成功）
    virtual void Stop();

    virtual void SetPause(bool is_pause);

    virtual bool IsPause();

    //入口主函数
    virtual void Main();

protected:
    bool is_exit_ = false;
    bool is_running_ = false;
    bool is_pause_ = false;
    bool is_pausing_ = false;

private:
    void ThreadMain();
};


#endif //XPLAY_XTHREAD_H
