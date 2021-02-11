#include "mainwindow.h"
#include "CrashDump.h"

#include <QApplication>

int main(int argc, char *argv[])
{
    CrashDump::init("Test");

    QApplication a(argc, argv);
    MainWindow w;
    w.show();
    return a.exec();
}
