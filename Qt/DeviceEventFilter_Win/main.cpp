#include <QApplication>
#include "mainwindow.h"
#include "DeviceEventFilter.h"

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);
    MainWindow w;
    w.show();

    DeviceEventFilter filter;
    filter.installFilter((HANDLE)w.winId());
    app.installEventFilter(&filter);

    return app.exec();
}
