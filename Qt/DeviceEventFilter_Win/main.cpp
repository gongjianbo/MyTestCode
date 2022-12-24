#include <QApplication>
#include "mainwindow.h"
#include "DeviceEventFilter.h"

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);
    MainWindow w;
    DeviceEventFilter filter;
    filter.installFilter((HANDLE)w.winId());
    app.installEventFilter(&filter);
    w.show();
    return app.exec();
}
