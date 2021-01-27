#pragma once

//定义宏使输出文件名和行号
#define SPDLOG_ACTIVE_LEVEL SPDLOG_LEVEL_TRACE
#define SPDLOG_TRACE_ON
//#define SPDLOG_DEBUG_ON

#include "spdlog/spdlog.h"
#include "spdlog/sinks/easy_file_sink.h"
//#include "spdlog/sinks/daily_file_sink.h"

//spdlog 简易封装
//龚建波 2021-1-27 修改
//''' 示例
// Qt使用时 CONFIG += utf8_source
// INITLOG(QString("日志.log").toLocal8Bit().toStdString());
// ERRORLOG("测试下 {0} {1} {2} {3}",123,true,"测试下",QString("测试下").toStdString());
// WARNLOG("测试下 {0} {1} {2} {3}",123,true,"测试下",QString("测试下").toStdString());
// INFOLOG("测试下 {0} {1} {2} {3}",123,true,"测试下",QString("测试下").toStdString());
// DEBUGLOG("测试下 {0} {1} {2} {3}",123,true,"测试下",QString("测试下").toStdString());
// TRACELOG("测试下 {0} {1} {2} {3}",123,true,"测试下",QString("测试下").toStdString());
//'''
//spdlog讲解参见博客 https://www.cnblogs.com/oucsheep/p/8426548.html
class BaseLog
{
private:
    BaseLog()=default;
public:
    static BaseLog* getInstance() {
        static BaseLog instance;
        return &instance;
    }

    //初始化日志，路径使用locale编码
    //如: QString("日志.log").toLocal8Bit().toStdString()
    void init(const std::string& path){
        logPtr = spdlog::easy_logger_mt("easy",path,1024*1024*25);

        //可以配置多个sink
        //std::make_shared<spdlog::logger>
        //spdlog::register_logger(logPtr); 配合 spdlog::drop_all();

        //设置日志记录级别
        logPtr->set_level(spdlog::level::trace);
        //设置格式
        //参见文档 https://github.com/gabime/spdlog/wiki/3.-Custom-formatting
        //[%Y-%m-%d %H:%M:%S.%e] 时间
        //[%l] 日志级别
        //[%t] 线程
        //[%s] 文件
        //[%#] 行号
        //[%!] 函数
        //[%v] 实际文本
        logPtr->set_pattern("[%Y-%m-%d %H:%M:%S.%e] [%l] [%t] [%s %!:%#] %v");
        //设置当出发 err 或更严重的错误时立刻刷新日志到  disk
        logPtr->flush_on(spdlog::level::trace);

        //spdlog::flush_every(std::chrono::seconds(3));
    }

    auto logger() {
        return logPtr;
    }

private:
    std::shared_ptr<spdlog::logger> logPtr;
};

#define INITLOG(path)     BaseLog::getInstance()->init(path)
//参见SPDLOG_LOGGER_CALL
#define SPDLOG_BASE(logger, level, ...) (logger)->log(spdlog::source_loc{__FILE__, __LINE__, __func__}, level, __VA_ARGS__)
#define TRACELOG(...)     SPDLOG_BASE(BaseLog::getInstance()->logger(), spdlog::level::trace, __VA_ARGS__)
#define DEBUGLOG(...)     SPDLOG_BASE(BaseLog::getInstance()->logger(), spdlog::level::debug, __VA_ARGS__)
#define INFOLOG(...)      SPDLOG_BASE(BaseLog::getInstance()->logger(), spdlog::level::info, __VA_ARGS__)
#define WARNLOG(...)      SPDLOG_BASE(BaseLog::getInstance()->logger(), spdlog::level::warn, __VA_ARGS__)
#define ERRORLOG(...)     SPDLOG_BASE(BaseLog::getInstance()->logger(), spdlog::level::err, __VA_ARGS__)
#define CRITICALLOG(...)  SPDLOG_BASE(BaseLog::getInstance()->logger(), spdlog::level::critical, __VA_ARGS__)
