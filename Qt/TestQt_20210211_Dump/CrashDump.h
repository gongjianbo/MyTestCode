#pragma once

#include <QObject>
#include <QCalendar>
#include <QDateTime>
#include <QFileInfo>
#include <QDir>
#include <Windows.h>
#include <DbgHelp.h>
#pragma comment(lib, "DbgHelp")
#include <iostream>

static QString __module_name;

/**
 * @brief MSVC+Qt 生成dump文件
 * @author 龚建波
 * @date 2021-2-11
 * @details 配合pdb文件使用，生成exe时记得生成dpb
 */
class CrashDump
{
public:
    //初始化注册
    static void init(const QString &module)
    {
        __module_name=module;
        //异常捕获
        ::SetUnhandledExceptionFilter((LPTOP_LEVEL_EXCEPTION_FILTER)exceptionFilter);
    }

    //获取exe路径
    static QString getAppPath()
    {
        DWORD v;
        QVarLengthArray<char, 2048> buffer;
        size_t size = 0;
        do {
            size += 2048;
            buffer.resize((int)size);
            v = ::GetModuleFileNameA(NULL, buffer.data(), DWORD(buffer.size()));
        } while (v >= size);

        return QString::fromLocal8Bit(buffer.data(), v);
    }

    //生成dump路径，可以自定义路径以及命名格式
    static QString getDumpPath()
    {
        QFileInfo app_info(getAppPath());
        QString dump_path=QString("%1/dump/%2 %3.dmp")
                .arg(app_info.absolutePath())
                .arg(__module_name)
                .arg(QDateTime::currentDateTime().toString("yyyy_MM_dd hh.mm.ss"));
        QFileInfo dump_info(dump_path);
        if(!dump_info.dir().exists()&&!dump_info.dir().mkpath(dump_info.absolutePath()))
            dump_path=QString("%1.dump").arg(__module_name);
        return dump_path;
    }

    //异常处理
    static LONG exceptionFilter(LPEXCEPTION_POINTERS lpExceptionInfo)
    {
        QString dump_path = getDumpPath();
        std::cerr<<"crash dump:"<<dump_path.toStdString()<<std::endl;

        //创建文件
        HANDLE file_handle = ::CreateFileA(dump_path.toLocal8Bit().data(), GENERIC_WRITE, 0, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
        if (file_handle != INVALID_HANDLE_VALUE) {
            DWORD dump_type = MiniDumpNormal; //可以加上需要的其他枚举值
            MINIDUMP_EXCEPTION_INFORMATION dump_info;
            dump_info.ClientPointers = TRUE;
            dump_info.ExceptionPointers = lpExceptionInfo;
            dump_info.ThreadId = ::GetCurrentThreadId();
            //写入dump
            ::MiniDumpWriteDump(GetCurrentProcess(), GetCurrentProcessId(), file_handle, (MINIDUMP_TYPE)dump_type, &dump_info, NULL, NULL);
        }
        ::CloseHandle(file_handle);
        //这里可以把程序拉起来
        return EXCEPTION_CONTINUE_SEARCH; //or EXCEPTION_EXECUTE_HANDLER
    }
};
