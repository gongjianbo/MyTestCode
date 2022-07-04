#include "MainWindow.h"
#include "ProcessManager.h"
#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);
    //初始化进程管理
    ProcessManager::getInstance()->init(5);

    MainWindow w;
    w.show();
    return app.exec();
}
