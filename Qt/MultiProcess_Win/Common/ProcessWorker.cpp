#include "ProcessWorker.h"

#include <QThread>
#include <QDebug>

ProcessWorker::ProcessWorker(QObject *parent)
    : QObject(parent)
{   
}

ProcessWorker::~ProcessWorker()
{
    freeWorker();
}

ProcessWorker *ProcessWorker::getInstance()
{
    static ProcessWorker instance;
    return &instance;
}

bool ProcessWorker::init(DWORD pid)
{
    // 目前仅在初始化时调用一次，已经初始化了就返回
    if (guardThread)
        return false;
    processPid = pid;
    if (pidActive(pid)) {
        initWorker();
        return true;
    }
    return false;
}

bool ProcessWorker::isActive() const
{
    return guardFlag;
}

void ProcessWorker::initWorker()
{
    guardFlag = true;
    guardThread = new std::thread([this] {
        int counter = 0;
        const int ms_sleep = 10;
        // 500ms查询一次manager是否还正常工作
        const int ms_guard = 500;
        while (guardFlag) {
            // 达到巡检时间间隔就重置count，并检测进程状态
            counter += ms_sleep;
            if (counter >= ms_guard) {
                counter = 0;
                if (!pidActive(processPid)) {
                    guardFlag = false;
                    emit managerFinished();
                    break;
                }
            }
            QThread::msleep(ms_sleep);
        }
    });
}

void ProcessWorker::freeWorker()
{
    if (!guardThread)
        return;
    guardFlag = false;
    if (guardThread->joinable()) {
        guardThread->join();
    }
}

bool ProcessWorker::pidActive(DWORD pid) const
{
    HANDLE h_process = ::OpenProcess(PROCESS_ALL_ACCESS, FALSE, pid);
    if (h_process != NULL) {
        DWORD exit_code;

        // 检测进程是否正常运行
        ::GetExitCodeProcess(h_process, &exit_code);
        ::CloseHandle(h_process);
        if (exit_code == STILL_ACTIVE) {
            qDebug() << (unsigned long)pid << "active";
            return true;
        }
    }
    qDebug() << (unsigned long)pid << "die";
    return false;
}
