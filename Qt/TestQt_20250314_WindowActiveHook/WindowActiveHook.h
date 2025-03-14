#pragma once
#include <Windows.h>
#include <functional>

// 监测窗口活跃状态
class WindowActiveHook
{
public:
    WindowActiveHook();
    ~WindowActiveHook();

    // - wid 关联的窗口
    // - interval 判定非活跃的超时时间
    // - callback 传递活跃or空闲状态的回调函数
    void registerHook(HWND wid, ULONGLONG interval, std::function<void (bool)> callback);
    void unregisterHook();

private:
    bool registered{false};
};
