#include "ClientWindow.h"

#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);
    ClientWindow w;
    w.show();
    return app.exec();
}
