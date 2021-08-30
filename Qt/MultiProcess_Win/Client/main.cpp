#include "ClientWindow.h"
#include "ProcessManager.h"
#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);
    //初始化进程管理
    ProcessManager::getInstance()->init(5);

    ClientWindow w;
    w.show();
    return app.exec();
}
