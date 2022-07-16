#include <QApplication>
#include "GLView.h"

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    GLView w;
    w.resize(900,600);
    w.setWindowTitle("Test Assimp");
    w.show();
    return a.exec();
}
