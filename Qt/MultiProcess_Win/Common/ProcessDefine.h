#pragma once
#include <Windows.h>
#include <QObject>

// 每个进程标记的信息，便于结束和重启
struct ProcessInfo
{
    // worker路径
    QString path;
    // 唯一标识，也是table.key
    // 为空时使用path作为key，这样的话就不能管理一个exe多次启动
    QString key;
    // 启动参数
    QStringList args;
    // 区分SW_SHOW和SW_HIDE
    bool visible{ false };
    // 进程句柄
    HANDLE hProcess{ NULL };
    // 自动重启
    bool autoRestart{ true };
    // 重启计数，每次正常置零，autoRestart=true则异常+1，达到预定值再重启
    // 加一个计数只是防止异常情况
    int restartCounter{ 0 };
};
