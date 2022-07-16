#include <QCoreApplication>
#include <QDebug>
#include "SharedObject.h"

int main(int argc, char *argv[])
{
    QCoreApplication app(argc, argv);
    {
        SharedObject b1;
        b1.setNum(11);
        b1.setName("11");
        SharedObject b2 = b1;
        SharedObject b3 = b1;
        qDebug()<<"before set";
        b1.setNum(22);
        qDebug()<<"after set";
        qDebug()<<b1.getNum()<<b1.getName()
               <<b2.getNum()<<b2.getName()
              <<b3.getNum()<<b3.getName();
    }
    return app.exec();
}
