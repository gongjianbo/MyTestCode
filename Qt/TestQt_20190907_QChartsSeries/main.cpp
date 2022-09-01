#include "MainWidget.h"

#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication::setAttribute(Qt::AA_DisableHighDpiScaling,true);

    QApplication a(argc, argv);
    MainWidget w;
    w.show();
    return a.exec();
}
