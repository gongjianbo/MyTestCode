#include <QApplication>
#include "MyTextEdit.h"

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);
    MyTextEdit w;
    w.resize(500, 300);
    w.setWindowTitle("GongJianBo 1992");
    w.show();
    return app.exec();
}
