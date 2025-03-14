#include <QApplication>
#include <QMainWindow>
#include <QDebug>
#include "WindowActiveHook.h"

class MainWindow : public QMainWindow
{
public:
    MainWindow() {
        // 超时或进入后台则为非活跃状态
        hook.registerHook((HWND)winId(), 3000, [](bool active){
            qDebug() << "active" << active;
        });
    }
private:
    WindowActiveHook hook;
};

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);
    MainWindow w;
    w.resize(600, 500);
    w.show();
    return app.exec();
}
