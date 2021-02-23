#include <QApplication>
#include <QDebug>

#include "form.h"

//简易的多进程管理
int main(int argc, char *argv[])
{
    QApplication app(argc, argv);

    for(int i=0;i<argc;i++)
        qDebug()<<QString(argv[i]);
    if(argc>1){
        DWORD manager_pid = QString(argv[1]).toULong();
        if(ProcessWorker::getInstance()->init(manager_pid)){
            QObject::connect(ProcessWorker::getInstance(),&ProcessWorker::managerFinished,
                    &app,&QCoreApplication::quit);
        }
    }

    //初始化进程管理
    ProcessManager::getInstance()->init(5);

    Form w;
    w.show();

    return app.exec();
}
