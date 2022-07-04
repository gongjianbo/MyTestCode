#pragma once
#include <QHash>
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
 * @todo
 * 1.通过event或者shared memory标记当前进程是否处于活跃状态
 * 如果卡住也需要重新启动
 * 2.start之前可以先kill该exe进程
 */
class ProcessManager : public QObject
{
    Q_OBJECT
private:
    explicit ProcessManager(QObject *parent = nullptr);
    Q_DISABLE_COPY_MOVE(ProcessManager)
public:
    ~ProcessManager();

    //单例操作，每个应用只有一个manager和worker
    static ProcessManager *getInstance();

    //初始化，启动时调用一次
    //limit=worker进程的最大限制数
    bool init(int limit = 10);
    //是否处于活动状态
    //未初始化默认false
    //init时置为true
    //释放时置为false
    bool isActive() const;

    //获取exe所在目录
    Q_INVOKABLE static QString getAppPath();

    //启动进程
    //exePath: 可执行文件的路径
    //key: 做为表唯一键，用以区分相同文件多次启动
    //argList: 启动参数
    //visible: 如果要显示界面则置位true，对应SW_SHOW
    //autoRestart: 循环检测进程是否正常运行，异常时会重启该进程
    bool startProcess(const QString &exePath,
                      const QString &key = QString(),
                      const QStringList &argList = QStringList(),
                      bool visible = false,
                      bool autoRestart = true);

    //结束进程
    //key: 做为表唯一键，用以区分相同文件多次启动
    //     如果startProcess没填key，也可以传exe路径来查找
    bool stopProcess(const QString &key);

private:
    //启动进程
    bool doStart(const ProcessInfo &proInfo);
    //结束进程
    //tableKey: processTable的键，key参数空则使用exePath作为键
    bool doStop(const QString &tableKey);
    //退出轮询线程
    void freeGuard();
    //轮询检测并重启autoRestart=true的异常进程
    void initGuard();
    //轮询状态，guardProcess中调用
    void patrol();
    //更新当前管理的进程key列表
    void updateKeys();

signals:
    //某个进程异常结束了
    //todo 因为检测多次后才重启，所以会重复触发该信号
    void processCrashed(const QString &exePath, const QString &key);
    //某个进程重启了
    void processRestarted(const QString &exePath, const QString &key);
    //进程列表更新
    void keysChanged(const QStringList &keys);

private:
    //预设进程数上限
    int processLimit{ 10 };
    //管理的进程列表
    QHash<QString,ProcessInfo> processTable;
    //会在多线程环境调用操作接口
    std::mutex theMtx;
    //轮询线程启停标志
    std::atomic_bool guardFlag{ false };
    //轮询线程
    std::thread *guardThread{ nullptr };
};

