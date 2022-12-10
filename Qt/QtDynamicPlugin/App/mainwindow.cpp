#include "mainwindow.h"
#include "WidgetInterface.h"
#include <QCoreApplication>
#include <QPushButton>
#include <QPluginLoader>
#include <QDir>
#include <QDebug>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
{
    setWindowTitle("Qt Dynamic Plugin");
    setCentralWidget(new QWidget(this));
    layout = new QVBoxLayout;
    centralWidget()->setLayout(layout);
    QPushButton *btn = new QPushButton("Main", this);
    connect(btn, &QPushButton::clicked, [=](){
        emit mainSignal("Main Emit Signal");
    });
    layout->addWidget(btn);

    loadPlugin();
}

MainWindow::~MainWindow()
{
}

void MainWindow::loadPlugin()
{
    QDir plugin_dir = QCoreApplication::applicationDirPath() + "/myplugins";
    const auto entry_list = plugin_dir.entryList(QStringList()<<"*.dll");
    for (const QString &file_name : entry_list) {
        qDebug()<<file_name<<plugin_dir.absoluteFilePath(file_name);
        QPluginLoader loader(plugin_dir.absoluteFilePath(file_name));
        //所以plugin至少得是QObject的子类
        QObject *plugin = loader.instance();
        if (plugin) {
            WidgetInterface *wi = qobject_cast<WidgetInterface*>(plugin);
            if (wi) {
                qDebug()<<"Load plugin"<<wi->pluginName();
                layout->addWidget(wi);
                connect(wi, &WidgetInterface::pluginSignal, this, &MainWindow::mainSlot);
                connect(this, &MainWindow::mainSignal, wi, &WidgetInterface::pluginSlot);
            }
        }
    }
}

void MainWindow::mainSlot(const QString &arg)
{
    qDebug()<<"Main Recv"<<arg;
}

