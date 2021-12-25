#include <QCoreApplication>
#include <QDebug>
#include "ByteArray.h"

int main(int argc, char *argv[])
{
    QCoreApplication app(argc, argv);
    {
        ByteArray b1;
        b1.setNum(11);
        b1.setName("11");
        ByteArray b2 = b1;
        ByteArray b3 = b1;
        qDebug()<<"before set";
        b1.setNum(22);
        qDebug()<<"after set";
        qDebug()<<b1.getNum()<<b1.getName()
               <<b2.getNum()<<b2.getName()
              <<b3.getNum()<<b3.getName();
    }
    return app.exec();
}
