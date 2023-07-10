#include "MainWindow.h"
#include "ui_MainWindow.h"
#include "CustomEvent.h"
#include <QCoreApplication>
#include <QDebug>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow{parent}
    , ui{new Ui::MainWindow}
{
    ui->setupUi(this);

    connect(ui->btnSend, &QPushButton::clicked, [this](){
        qDebug() << "before send event";
        CustomEvent custom{"by send"};
        // send event 需要自己释放
        QCoreApplication::sendEvent(this, &custom);
        qDebug() << "after send event";
    });

    connect(ui->btnPost, &QPushButton::clicked, [this](){
        qDebug() << "before post event";
        // post event 交给队列管理
        QCoreApplication::postEvent(this, new CustomEvent{"by post"});
        qDebug() << "after post event";
    });
}

MainWindow::~MainWindow()
{
    delete ui;
}

bool MainWindow::event(QEvent *evt)
{
    if (evt->type() == CustomEvent::IsCustom)
    {
        CustomEvent *custom = static_cast<CustomEvent *>(evt);
        if (custom) {
            // 设置 accept，避免被当作未处理的继续往 parent 传递
            custom->accept();
            qDebug() << __FUNCTION__ << custom->message() << custom->type();
            return true;
        }
    }
    return QMainWindow::event(evt);
}

