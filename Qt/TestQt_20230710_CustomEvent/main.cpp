#include "MyApplication.h"
#include "MainWindow.h"

int main(int argc, char *argv[])
{
    MyApplication app(argc, argv);
    MainWindow w;
    w.show();
    return app.exec();
}
