#include "ProcessManager.h"

#include <QFile>
#include <QFileInfo>
#include <QDebug>
#include <chrono>
#include <shellapi.h>

//ShellExecuteEx
#pragma comment(lib,"shell32.lib")

ProcessManager::ProcessManager(QObject *parent)
    : QObject(parent)
{
}

ProcessManager::~ProcessManager()
{
    freeGuard();
}

ProcessManager *ProcessManager::getInstance()
{
    static ProcessManager instance;
    return &instance;
}

bool ProcessManager::init(int limit)
{
    if(limit<1){
        qDebug()<<"init process manager error."<<limit;
        return false;
    }
    processLimit = limit;

    //启动后定时巡检
    initGuard();
    qDebug()<<"init process manager success.";
    return true;
}

QString ProcessManager::getAppPath()
{
    DWORD v;
    QVarLengthArray<char,2048> buffer;
    size_t size = 0;
    do {
        size += 2048;
        buffer.resize((int)size);
        v = ::GetModuleFileNameA(NULL, buffer.data(), DWORD(buffer.size()));
    } while (v >= size);

    QString app_path = QString::fromLocal8Bit(buffer.data(), v);
    QFileInfo app_fileinfo(app_path);

    return app_fileinfo.absolutePath();
}

bool ProcessManager::startProcess(const QString &exePath, const QString &key, const QStringList &argList, bool autoRestart)
{
    std::lock_guard<std::mutex> guard(theMtx); (void)guard;
    if(processLimit <= processTable.count()){
        qDebug()<<"out of process limit."<<processLimit;
        return false;
    }
    const QString table_key = key.isEmpty()?exePath:key;
    if(processTable.contains(table_key)){
        qDebug()<<"process key already exists.";
        return false;
    }

    ProcessInfo info;
    info.path = exePath;
    info.key = key;
    info.args = argList;
    info.hProcess = NULL;
    info.autoRestart = autoRestart;
    info.restartCounter = 0;

    return doStart(info);
}

bool ProcessManager::stopProcess(const QString &exePath, const QString &key)
{
    std::lock_guard<std::mutex> guard(theMtx); (void)guard;
    qDebug()<<"terminate exe."<<exePath<<key;
    const QString table_key = key.isEmpty()?exePath:key;

    return doStop(table_key);
}

bool ProcessManager::doStart(const ProcessInfo &proInfo)
{
    ProcessInfo info = proInfo;
    //本来想传handle，但是DuplicateHandle参数没看明白，就传pid算了
    const unsigned long current_pid = (unsigned long)(DWORD)::GetProcessId(::GetCurrentProcess());
    //qDebug()<<"current pid"<<current_pid<<QString::number(current_pid);
    const QString info_arg = QString("\"%1\" %2").arg(QString::number(current_pid)).arg(info.args.join(" "));
    const QString table_key = info.key.isEmpty()?info.path:info.key;
    //byte放到外层，不然临时变量释放了，data里的数据也没了
    const QByteArray path_temp = info.path.toLocal8Bit();
    const QByteArray arg_temp = info_arg.toLocal8Bit();

    SHELLEXECUTEINFOA se_info;
    memset(&se_info, 0x00, sizeof(SHELLEXECUTEINFOA));
    //in.required.此结构体字节大小
    se_info.cbSize = sizeof(SHELLEXECUTEINFOA);
    //in.SEE_MASK_NOCLOSEPROCESS用于指示hProcess成员接收到进程句柄。
    se_info.fMask = SEE_MASK_NOCLOSEPROCESS;
    //in.optional.父窗口的句柄，用于显示系统在执行此功能时可能产生的任何消息框。该值可以为NULL。
    //se_info.hwnd = NULL;
    //in.optional.要执行的动作，open打开指定lpFile文件，runas管理员身份启动应用程序
    se_info.lpVerb = "open";
    //in.以空值结尾的字符串的地址
    se_info.lpFile = path_temp.constData();
    //in.optional.执行参数
    se_info.lpParameters = arg_temp.constData();
    //in.optional.工作目录，为NULL则使用当前目录
    //se_info.lpDirectory = NULL;
    //in.required.SW_HIDE隐藏该窗口并激活另一个窗口，打开的进程不会显示窗口
    //因为hide不显示窗口，测试/调试时可以用SW_SHOW
    se_info.nShow = SW_SHOW;
    //out.如果设置了SEE_MASK_NOCLOSEPROCESS并且ShellExecuteEx调用成功，它将把该成员设置为大于32的值。
    //如果函数失败，则将其设置为SE_ERR_XXX错误值，以指示失败的原因。
    //se_info.hInstApp;
    //in.valid when SEE_MASK_IDLIST
    //se_info.lpIDList;
    //in.valid when SEE_MASK_CLASSNAME
    //se_info.lpClass;
    //in.valid when SEE_MASK_CLASSKEY
    //se_info.hkeyClass;
    //in.valid when SEE_MASK_HOTKEY
    //se_info.dwHotKey;
    //一个union，hMonitor in.valid when SEE_MASK_HMONITOR
    //se_info.DUMMYUNIONNAME;
    //out.valid when SEE_MASK_NOCLOSEPROCESS 新启动的应用程序的句柄，如果未启动则为NULL
    //se_info.hProcess;

    qDebug()<<"execute exe."<<info.path<<info.key<<info.args;
    //ShellExecuteEx创建的进程可以提权
    //CreateProcess继承权限，但可以更好地控制
    if(::ShellExecuteExA(&se_info)){
        if(se_info.hProcess != NULL){
            info.hProcess = se_info.hProcess;
            info.restartCounter = 0;
            processTable[table_key] = info;
            qDebug()<<"execute success.";
            return true;
        }
    }
    qDebug()<<"execute fail."<<(int)(HINSTANCE)se_info.hInstApp;
    return false;
}

bool ProcessManager::doStop(const QString &tableKey)
{
    if(processTable.contains(tableKey)){
        //take并退出进程，回收offset
        ProcessInfo p_info = processTable.take(tableKey);
        if(::TerminateProcess(p_info.hProcess,-1)){
            qDebug()<<"terminate success.";
            return true;
        }
    }
    return false;
}

void ProcessManager::freeGuard()
{
    if(!guardFlag||!guardThread)
        return;
    guardFlag = false;
    if(guardThread->joinable()){
        guardThread->join();
    }
    qDebug()<<"free process manager success.";
}

void ProcessManager::initGuard()
{
    if(guardFlag || guardThread)
        return;
    guardFlag = true;
    guardThread = new std::thread([this]{
        int counter = 0;
        const int ms_sleep = 10;
        const int ms_guard = 500;
        while(guardFlag){
            //达到巡检时间间隔就重置count，并检测进程状态
            counter += ms_sleep;
            if(counter >= ms_guard){
                counter = 0;
                patrol();
            }
            std::this_thread::sleep_for(std::chrono::milliseconds(ms_sleep));
        }

        //全部退出，放到子进程去判断主进程是否还存在
        //QList<QString> quit_keys;
        //for(auto iter=processTable.begin();iter!=processTable.end();iter++)
        //{
        //    const ProcessInfo node = iter.value();
        //    const QString table_key = node.key.isEmpty()?node.path:node.key;
        //    quit_keys.push_back(table_key);
        //}
        //for(QString table_key : quit_keys)
        //    doStop(table_key);
    });
}

void ProcessManager::patrol()
{
    std::lock_guard<std::mutex> guard(theMtx); (void)guard;

    //退出且不重启的进程在遍历完就关闭
    QList<QString> quit_keys;

    //检查所有进程的状态，并重置计数
    for(auto iter=processTable.begin();iter!=processTable.end();iter++)
    {
        const ProcessInfo node = iter.value();
        DWORD exit_code;

        //检测进程是否正常运行，把需要重启的重启
        ::GetExitCodeProcess(node.hProcess,&exit_code);
        //如果进程尚未终止且函数成功，则返回的状态为STILL_ACTIVE
        if(exit_code!=STILL_ACTIVE){
            qDebug()<<"process crash."<<node.path<<node.key;
            emit processCrashed(node.path,node.key);
            //如果是自动重启的就重启，否则从列表移除
            if(node.autoRestart){
                iter->restartCounter += 1;
                //加一个计数只是防止异常情况
                if(iter->restartCounter>=3 && doStart(node)){
                    qDebug()<<"process restart."<<node.path<<node.key;
                    emit processRestarted(node.path,node.key);
                }
            }else{
                const QString table_key = node.key.isEmpty()?node.path:node.key;
                quit_keys.push_back(table_key);
            }
        }else{
            //TODO 通过event或者shared memory查询当前进程是否处于活跃状态
            if(node.autoRestart){
                iter->restartCounter = 0;
            }
        }
    }

    for(QString table_key : quit_keys)
        doStop(table_key);
}
