#include "WindowActiveHook.h"
#include <QDebug>
#pragma comment(lib, "user32.lib")

// 关联的窗口
HWND hookWid = NULL;
// 判定非活跃的超时时间
ULONGLONG hookInterval = 0;
// 状态回调
std::function<void (bool)> hookCallback;
// 时间戳
ULONGLONG tickTime = 0;
// 当前窗口是否为前台
bool windowForeground = false;
// 当前窗口是否活跃
bool windowActive = false;
// 窗口消息钩子
HHOOK windowHook = NULL;
// 定时器句柄
UINT_PTR timerId = NULL;
// 键盘消息钩子
HHOOK keyboardHook = NULL;
// 鼠标消息钩子
HHOOK mouseHook = NULL;
// 定时器回调
void CALLBACK TimerProcess(HWND hWnd, UINT msg, UINT_PTR idTimer, DWORD dwTime);
// 键盘消息回调
LRESULT CALLBACK KeyboardHookProcess(int nCode, WPARAM wParam, LPARAM lParam);
// 鼠标消息回调
LRESULT CALLBACK MouseHookProcess(int nCode, WPARAM wParam, LPARAM lParam);

// 窗口消息回调
LRESULT CALLBACK WindowHookProcess(int nCode, WPARAM wParam, LPARAM lParam)
{
    // - nCode 钩子代码，指示当前钩子事件的类型
    // - wParam 事件类型，表示键盘或鼠标事件的具体操作
    // - lParam 事件数据，指向具体的输入信息结构体
    if (nCode >= 0 && lParam)
    {
        // CWPSTRUCT结构体包含窗口消息信息
        // - lParam 消息的附加信息
        // - wParam 消息的附加信息
        // - message 消息编号
        // - hwnd 目标窗口的句柄
        CWPSTRUCT *evt = (CWPSTRUCT*)lParam;
        HWND hwnd = evt->hwnd;
        UINT message = evt->message;
        if (message == WM_ACTIVATE) {
            if (evt->wParam == WA_ACTIVE || evt->wParam == WA_CLICKACTIVE) {
                qDebug() << "Window foreground" << hwnd << GetTickCount64();
                if (!windowForeground) {
                    tickTime = GetTickCount64();
                    windowForeground = true;
                    keyboardHook = SetWindowsHookEx(WH_KEYBOARD_LL, KeyboardHookProcess, GetModuleHandle(NULL), NULL);
                    mouseHook = SetWindowsHookEx(WH_MOUSE_LL, MouseHookProcess, GetModuleHandle(NULL), NULL);
                    timerId = SetTimer(NULL, 0, 50, TimerProcess);
                    windowActive = true;
                    if (hookCallback) hookCallback(true);
                }
            } else {
                qDebug() << "Window background" << hwnd << GetTickCount64();
                if (windowForeground) {
                    windowForeground = false;
                    UnhookWindowsHookEx(keyboardHook);
                    keyboardHook = NULL;
                    UnhookWindowsHookEx(mouseHook);
                    mouseHook = NULL;
                    KillTimer(NULL, timerId);
                    timerId = NULL;
                    windowActive = false;
                    if (hookCallback) hookCallback(false);
                }
            }
        }
    }
    return CallNextHookEx(windowHook, nCode, wParam, lParam);
}

// 定时器回调
void CALLBACK TimerProcess(HWND /*hWnd*/, UINT /*msg*/, UINT_PTR idTimer, DWORD dwTime)
{
    if (timerId != idTimer) return;
    // qDebug() << __FUNCTION__ << dwTime << tickTime;
    if (windowActive && dwTime - tickTime >= hookInterval) {
        windowActive = false;
        if (hookCallback) hookCallback(false);
    } else if (!windowActive && dwTime - tickTime < hookInterval && windowForeground) {
        windowActive = true;
        if (hookCallback) hookCallback(true);
    }
}

// 键盘消息回调
LRESULT CALLBACK KeyboardHookProcess(int nCode, WPARAM wParam, LPARAM lParam)
{
    // - nCode 钩子代码，指示当前钩子事件的类型
    // - wParam 事件类型，表示键盘或鼠标事件的具体操作
    //   WM_KEYDOWN 普通按键按下
    //   WM_KEYUP 普通按鍵抬起
    //   WM_SYSKEYDOWN 系统按键按下
    //   WM_SYSKEYUP 系统按键抬起
    // - lParam 事件数据，指向具体的输入信息结构体
    if (nCode >= 0 && wParam && lParam)
    {
        // KBDLLHOOKSTRUCT结构体包含键盘输入事件信息
        // - vkCode 虚拟键码
        // - scanCode 硬件扫描码（有些键值相同）
        // - flags 事件标志，用于标识特殊键（如扩展键、按键释放等）
        //   LLKHF_EXTENDED/LLKHF_INJECTED/LLKHF_ALTDOWN/LLKHF_UP
        // - time 消息时间戳，可以用GetTickCount64()获取对应的系统启动时间戳
        // - dwExtraInfo 与消息关联的其他信息
        KBDLLHOOKSTRUCT *evt = (KBDLLHOOKSTRUCT*)lParam;
        tickTime = evt->time;
        qDebug() << "Keyboard" << nCode << wParam << evt->vkCode << evt->scanCode << evt->flags << evt->time;
    }
    return CallNextHookEx(keyboardHook, nCode, wParam, lParam);
}

// 鼠标消息回调
LRESULT CALLBACK MouseHookProcess(int nCode, WPARAM wParam, LPARAM lParam)
{
    // - nCode 钩子代码，指示当前钩子事件的类型
    // - wParam 事件类型，表示键盘或鼠标事件的具体操作
    //   WM_MOUSEMOVE 鼠标移动
    //   WM_MOUSEWHEEL 滚轮滚动
    //   WM_LBUTTONDOWN 左键按下
    //   WM_RBUTTONDOWN 右键按下
    // - lParam 事件数据，指向具体的输入信息结构体
    if (nCode >= 0 && wParam && lParam)
    {
        // MSLLHOOKSTRUCT结构体包含键盘输入事件信息
        // - pt 鼠标的屏幕坐标 (x, y)
        // - mouseData 额外的鼠标信息 (滚轮滚动、X 按键)
        // - flags 事件标志，用于标识特殊键（如扩展键、按键释放等）
        // - time 消息时间戳，可以用GetTickCount64()获取对应的系统启动时间戳
        // - dwExtraInfo 与消息关联的其他信息
        MSLLHOOKSTRUCT *evt = (MSLLHOOKSTRUCT*)lParam;
        tickTime = evt->time;
        // qDebug() << "Mouse" << nCode << wParam << evt->pt.x << evt->pt.y << evt->mouseData << evt->flags << evt->time;
    }
    return CallNextHookEx(mouseHook, nCode, wParam, lParam);
}

WindowActiveHook::WindowActiveHook()
{

}

WindowActiveHook::~WindowActiveHook()
{
    unregisterHook();
}

void WindowActiveHook::registerHook(HWND wid, ULONGLONG interval, std::function<void (bool)> callback)
{
    if (registered) return;
    qDebug() << __FUNCTION__ << wid;
    registered = true;
    hookWid = wid;
    hookInterval = interval;
    hookCallback = callback;
    windowHook = SetWindowsHookEx(WH_CALLWNDPROC, WindowHookProcess, NULL, GetWindowThreadProcessId(hookWid, NULL));
    if (hookWid == GetForegroundWindow()) {
        qDebug() << __FUNCTION__ << "foreground";
        tickTime = GetTickCount64();
        windowForeground = true;
        keyboardHook = SetWindowsHookEx(WH_KEYBOARD_LL, KeyboardHookProcess, GetModuleHandle(NULL), NULL);
        mouseHook = SetWindowsHookEx(WH_MOUSE_LL, MouseHookProcess, GetModuleHandle(NULL), NULL);
        windowActive = true;
        // SetTimer创建定时器
        // - hWnd 指定关联窗口
        // - nIDEvent 定时器标识符，0自动生成
        // - uElapse 间隔时间，毫秒
        // - lpTimerFunc 指定回调函数
        timerId = SetTimer(NULL, 0, 50, TimerProcess);
    }
}

void WindowActiveHook::unregisterHook()
{
    if (!registered) return;
    qDebug() << __FUNCTION__;
    registered = false;
    hookWid = NULL;
    hookInterval = 0;
    hookCallback = std::function<void (bool)>();
    tickTime = 0;
    windowForeground = false;
    UnhookWindowsHookEx(windowHook);
    windowHook = NULL;
    UnhookWindowsHookEx(keyboardHook);
    keyboardHook = NULL;
    UnhookWindowsHookEx(mouseHook);
    mouseHook = NULL;
    KillTimer(NULL, timerId);
    windowActive = false;
    timerId = NULL;
}
