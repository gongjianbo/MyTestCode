#include "MessageObject.h"
#include <QDebug>
#include <Windows.h>

// 传递的数据
struct MessageObjectData
{
    int flag;
    std::string msg;

    // 结构体序列化为字节数组
    std::string toString()
    {
        std::string flag_buf;
        flag_buf.resize(sizeof(int));
        memcpy((void *)flag_buf.data(), (const void *)&flag, sizeof(int));
        return flag_buf + msg;
    }
    // 从字节数组反序列化
    static MessageObjectData fromString(const std::string &buf)
    {
        std::string temp = buf;
        MessageObjectData data = {0};
        if (temp.size() < sizeof(int))
            return data;
        memcpy((void *)&data.flag, (const void *)temp.data(), sizeof(int));
        temp = temp.substr(sizeof(int));
        data.msg = temp;
        return data;
    }
};

class MessageObjectPrivate
{
public:
    // 本地窗口标识
    std::wstring localTag;
    // 本地窗口
    HWND localHwnd{ nullptr };
    // 通信窗口标识
    std::wstring remoteTag;
    // 通信窗口
    // HWND remoteHwnd{ nullptr };

    // 处理窗口消息
    static LRESULT CALLBACK windowMessageProcess(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
    {
        switch (uMsg)
        {
        case WM_COPYDATA: {
            std::string recv_buf;
            // 接收传递的字节数组
            COPYDATASTRUCT *pcpd = (COPYDATASTRUCT *)lParam;
            if (!pcpd || pcpd->cbData <= 0)
                break;
            // 反序列化成结构体
            recv_buf = std::string((const char *)pcpd->lpData, pcpd->cbData);
            MessageObjectData data = MessageObjectData::fromString(recv_buf);
            int flag = (int)wParam;
            qDebug() << hwnd << "recv custom message" << flag << pcpd->dwData << data.flag << QString::fromStdString(data.msg);
            return 0;
        }
        }
        // 默认处理
        return ::DefWindowProc(hwnd, uMsg, wParam, lParam);
    }

    // 窗口类名
    static std::wstring windowClassName(const std::wstring &tag)
    {
        // std::to_wstring(::GetCurrentProcessId())
        return std::wstring(L"MessageObject_Window_" + tag);
    }

    bool init(const std::wstring &local, const std::wstring &remote)
    {
        localTag = local;
        remoteTag = remote;

        std::wstring class_name = windowClassName(localTag);
        // 注册窗口类
        HINSTANCE hi = ::GetModuleHandleW(nullptr);

        WNDCLASSW wc;
        memset(&wc, 0, sizeof(WNDCLASSW));
        wc.lpfnWndProc = windowMessageProcess;
        wc.cbClsExtra = 0;
        wc.cbWndExtra = 0;
        wc.hInstance = hi;
        wc.lpszClassName = class_name.c_str();
        ::RegisterClassW(&wc);

        localHwnd = ::CreateWindowW(wc.lpszClassName, // classname
                                    wc.lpszClassName, // window name
                                    0,  // style
                                    0,  // x
                                    0,  // y
                                    0,  // width
                                    0,  // height
                                    0,  // parent
                                    0,  // menu handle
                                    hi, // application
                                    0); // windows creation data.
        if (!localHwnd) {
            qDebug() << "createMessageWindow error" << (int)GetLastError();
            return false;
        } else {

        }
        ::SetWindowLongPtrW(localHwnd, GWLP_USERDATA, (LONG_PTR)this);
        return true;
    }

    void free()
    {
        if (localHwnd) {
            ::DestroyWindow(localHwnd);
            localHwnd = nullptr;
        }
        ::UnregisterClassW(windowClassName(localTag).c_str(), ::GetModuleHandleW(nullptr));
    }

    void sendMessage(int flag, const std::string &msg)
    {
        if (localHwnd && remoteTag.empty())
            return;
        // 查找接收端窗口
        std::wstring class_name = windowClassName(remoteTag);
        HWND remoteHwnd = ::FindWindowW(class_name.c_str(), class_name.c_str());
        if (!remoteHwnd)
            return;
        qDebug() << localHwnd << "send custom message" << flag << QString::fromStdString(msg) << remoteHwnd;
        MessageObjectData data;
        data.flag = flag;
        data.msg = msg;
        // 将结构体序列化成字节数组
        std::string send_buf = data.toString();
        // 用 WM_COPYDATA 发送字节数组
        COPYDATASTRUCT cpd;
        cpd.dwData = flag;
        cpd.cbData = send_buf.size();
        cpd.lpData = (void *)send_buf.data();
        ::SendMessageW(remoteHwnd, WM_COPYDATA, 0, (LPARAM)&cpd);
    }
};

MessageObject::MessageObject()
    : dptr{new MessageObjectPrivate}
{

}

MessageObject::~MessageObject()
{
    free();
}

void MessageObject::init(const std::wstring &local, const std::wstring &remote)
{
    const bool ret = dptr->init(local, remote);
    if (!ret) {
        dptr->free();
    }
}

void MessageObject::free()
{
    dptr->free();
}

void MessageObject::sendMessage(int flag, const std::string &msg)
{
    dptr->sendMessage(flag, msg);
}
