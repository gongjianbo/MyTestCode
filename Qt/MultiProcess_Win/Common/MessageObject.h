#pragma once
#include <string>
#include <memory>
#include <thread>
#include <atomic>
#include <mutex>
#include <stdio.h>

class MessageObjectPrivate;

/**
 * @brief 通过窗口消息通信
 * @author 龚建波
 * @date 2025-05-30
 * @details 在不同进程创建MessageObject对象，并通过唯一的窗口标识初始化
 */
class MessageObject
{
public:
    MessageObject();
    ~MessageObject();

    // 初始化
    // 通过local和remote两个标识来区分通信双方，标识符需唯一，用于收发窗口信息
    void init(const std::wstring &local, const std::wstring &remote);
    // 释放（析构时会自动调用）
    void free();

    // 发送消息
    void sendMessage(int flag, const std::string &msg);

private:
    std::shared_ptr<MessageObjectPrivate> dptr;
};
