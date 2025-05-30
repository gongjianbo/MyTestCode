#include "SubWindow.h"
#include "ProcessWorker.h"
#include <QApplication>
#include <QDebug>

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);
    int ret = -1;
    // 0-exe路径
    // 1-主进程pid
    // 2-服务名
    QList<QString> arguments = app.arguments();
    qDebug() << argc << ":" << arguments;
    if (arguments.size() != argc || argc < 3) {
        return ret;
    }

    // 第一个参数是exe路径，第二个开始才是启动参数
    DWORD manager_pid = arguments.at(1).toULong();
    if (ProcessWorker::getInstance()->init(manager_pid)) {
        // 检测到主进程退出就退出
        QObject::connect(ProcessWorker::getInstance(), &ProcessWorker::managerFinished,
                         &app, &QCoreApplication::quit);
        SubWindow w;
        w.setWindowTitle(w.windowTitle() + " : " + arguments.at(2) + " : " + arguments.at(3));
        w.show();
        return app.exec();
    }

    return ret;
}
