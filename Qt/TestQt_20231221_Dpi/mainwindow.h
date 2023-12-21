#pragma once
#include <QMainWindow>

QT_BEGIN_NAMESPACE
namespace Ui {
class MainWindow;
}
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

    void initWindow();
    void onScreenChange(QScreen *screen);
    void calcDpi();

private:
    Ui::MainWindow *ui;
    QMetaObject::Connection physicalDpiConnection;
    QMetaObject::Connection logicalDpiConnection;
};

