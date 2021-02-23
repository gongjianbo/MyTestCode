#pragma once

#include <thread>
#include <mutex>

#include "ProcessDefine.h"

/**
 * @brief 进程管理
 * @author 龚建波
 * @date 2021-2-23
 * @details
 * 1.manager和worker配对使用
 * 管理进程用manager，被管理的用worker
 * 2.每个进程只保持一个manager和worker
 */
class ProcessWorker : public QObject
{
    Q_OBJECT
private:
    explicit ProcessWorker(QObject *parent = nullptr);
public:
    ~ProcessWorker();

    //单例操作，每个应用只有一个manager和worker
    static ProcessWorker *getInstance();

    //根据pid初始化
    bool init(DWORD pid);

private:
    //轮询检测manager进程是否存在
    void initWorker();
    //正常退出
    void freeWorker();
    //检测manager进程
    bool pidActive(DWORD pid) const;

signals:
    //主进程退出了，提示本进程也可以结束了
    void managerFinished();

private:
    //manager进程的pid
    DWORD processPid = 0;
    //轮询线程启停标志
    std::atomic_bool guardFlag = false;
    //轮询线程
    std::thread *guardThread = nullptr;
};
