#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QCoreApplication>
#include <QSettings>
#include <QDateTime>
#include <QFileInfo>
#include <QMetaType>
#include <QDebug>
#include "EasyIni.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    connect(ui->myWrite,&QPushButton::clicked,this,[this]{
        using namespace Tool;
        const QString path=qApp->applicationDirPath()+"/ini/"+ui->myEdit->text();
        qDebug()<<"write"<<path;
        EasyIni ini(path,EasyIni::WriteOnly);
        ini.setValue("AA","AA","测试");
        ini.setValue("BB","AA","测试");
        ini.setValue("BB","BB",1);
        ini.setValue("BB","CC",true);
        ini.setValue("中文组","中文键","中文值");
    });
    connect(ui->myRead,&QPushButton::clicked,this,[this]{
        using namespace Tool;
        const QString path=qApp->applicationDirPath()+"/ini/"+ui->myEdit->text();
        qDebug()<<"read"<<path;
        EasyIni ini(path,EasyIni::ReadOnly);
        ini.dumpInfo();
        qDebug()<<ini.groups();
        qDebug()<<ini.keys("AA");
        qDebug()<<ini.keys("A");
        qDebug()<<ini.value("BB","AA");
        qDebug()<<ini.value("BB","BB");
        qDebug()<<ini.value("BB","CC");
        qDebug()<<ini.value("BB","QWER");
        ini.setValue("BB","CC",true);
        ini.setValue("BB","QWER",12345);
        qDebug()<<ini.value("BB","CC");
        qDebug()<<ini.value("BB","QWER");
        qDebug()<<ini.value("BB","QWER2");
    });

    connect(ui->btnWrite,&QPushButton::clicked,this,[this]{
        const QString path=qApp->applicationDirPath()+"/ini/"+ui->lineEdit->text();
        QSettings setting(path,QSettings::IniFormat);
        setting.setIniCodec("utf-8");
        qDebug()<<"write"<<path;
        setting.setValue("BB/AA","测试");
        setting.setValue("BB/BB",true);
        setting.setValue("BB/CC",123.7);
        //会把中文写成unicode：%U4E2D%U6587
        //但是手动输入的中文时不能读的，且写的时候会修改掉手写的中文
        setting.setValue("中文/中文","测试中文");
    });
    connect(ui->btnRead,&QPushButton::clicked,this,[this]{
        const QString path=qApp->applicationDirPath()+"/ini/"+ui->lineEdit->text();

        QFileInfo info(path);
        qDebug()<<info.lastModified()<<info.metadataChangeTime();

        QSettings setting(path,QSettings::IniFormat);
        setting.setIniCodec("utf-8");
        qDebug()<<"read"<<path;
        qDebug()<<setting.value("BB/AA");
        qDebug()<<setting.value("BB/BB");
        qDebug()<<setting.value("BB/CC");
        qDebug()<<setting.value("None/AA");
    });

    //qDebug()<<QVariant("false").toBool()<<QVariant("true").toBool();
}

MainWindow::~MainWindow()
{
    delete ui;
}

