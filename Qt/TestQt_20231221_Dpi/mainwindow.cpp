#include "mainwindow.h"
#include "ui_mainwindow.h"

#include <QDebug>
#include <QWindow>
#include <QScreen>
// 需要引入 gui-private 或者 quick-private
#include <qpa/qplatformscreen.h>
// #include <private/qhighdpiscaling_p.h>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    connect(ui->btnDpi, &QPushButton::clicked, this, &MainWindow::calcDpi);

    initWindow();
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::initWindow()
{
    qDebug()<<this->winId();
    // 构造的时候 windowHandle 还未初始化，可以等 show 的时候关联，或者先调用一次 winId
    connect(this->windowHandle(), &QWindow::screenChanged, this, &MainWindow::onScreenChange);
    calcDpi();
}

void MainWindow::onScreenChange(QScreen *screen)
{
    // QMetaObject::Connection 保存连接便于释放;
    if (logicalDpiConnection) {
        disconnect(physicalDpiConnection);
        disconnect(logicalDpiConnection);
    }
    if (screen) {
        // 切换缩放比时，EnableHighDpiScaling 会触发 physicalDotsPerInchChanged
        // 此时 Qt6 不触发 logicalDotsPerInchChanged
        physicalDpiConnection = connect(screen, &QScreen::physicalDotsPerInchChanged, this, &MainWindow::calcDpi);
        logicalDpiConnection = connect(screen, &QScreen::logicalDotsPerInchChanged, this, &MainWindow::calcDpi);
        calcDpi();
    }
}

void MainWindow::calcDpi()
{
    // 可以从 QWidget 或者 QWindow 拿到 QScreen 对象
    QScreen *cur_screen = this->screen();
    qDebug()<<__FUNCTION__;
    if (cur_screen) {
        // 逻辑 dpi (logicalBaseDpi().first) 默认值 win 96/ mac 72
        const QDpi base_dpi = cur_screen->handle()->logicalBaseDpi();
        const QDpi logic_dpi = QPlatformScreen::overrideDpi(cur_screen->handle()->logicalDpi());
        // 得到屏幕缩放百分比
        const qreal factor = qreal(logic_dpi.first) / qreal(base_dpi.first);
        qDebug()<<"calc dpi"<<factor;
    }
}
