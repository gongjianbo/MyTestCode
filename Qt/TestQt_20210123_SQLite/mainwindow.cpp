#include "mainwindow.h"
#include "ui_mainwindow.h"

#include <QtConcurrentRun>
#include <QSqlDatabase>
#include <QSqlQuery>
#include <QSqlError>
#include <QDateTime>
#include <QDebug>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    connect(ui->btnCreate,&QPushButton::clicked,this,&MainWindow::createTable);
    connect(ui->btnClear,&QPushButton::clicked,this,&MainWindow::clearTable);
    connect(ui->btnCount,&QPushButton::clicked,this,&MainWindow::searchCount);
    connect(ui->btnStart,&QPushButton::clicked,this,[=]{
        stopFlag=false;
        ui->btnStart->setEnabled(false);
        int thread_count=ui->threadBox->value();
        int loop_count=ui->loopBox->value();
        bool loop_lock=ui->lockBox->isChecked();
        for(int i=0;i<thread_count;i++)
        {
            QtConcurrent::run([=]{
                this->writeTest(i,loop_count,loop_lock);
            });
        }
    });

    //只是个简单的测试，start loop完后，手动点stop复位
    connect(ui->btnStop,&QPushButton::clicked,this,[=]{
        stopFlag=true;
        ui->btnStart->setEnabled(true);
    });
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::createTable()
{
    QSqlDatabase db;
    const QString connect_name="ConnectMain";
    if (QSqlDatabase::contains(connect_name)){
        db = QSqlDatabase::database(connect_name);
    }else{
        db = QSqlDatabase::addDatabase("QSQLITE",connect_name);
        db.setDatabaseName("Test.db");
    }

    if(db.open()){
        const QString sql=QString(R"(CREATE TABLE IF NOT EXISTS MY_TABLE(
ID    INTEGER    PRIMARY KEY AUTOINCREMENT UNIQUE,
IDX   INTEGER,
LOOP  INTEGER,
TIME  TEXT
);)");
        QSqlQuery query(db);
        bool result=query.exec(sql);
        qDebug()<<"create table"<<result;
        db.close();
    }
}

void MainWindow::clearTable()
{
    QSqlDatabase db;
    const QString connect_name="ConnectMain";
    if (QSqlDatabase::contains(connect_name)){
        db = QSqlDatabase::database(connect_name);
    }else{
        db = QSqlDatabase::addDatabase("QSQLITE",connect_name);
        db.setDatabaseName("Test.db");
    }

    if(db.open()){
        const QString sql=QString(R"(DELETE FROM MY_TABLE;)");
        QSqlQuery query(db);
        bool result=query.exec(sql);
        qDebug()<<"drop table"<<result;
        db.close();
    }
}

void MainWindow::searchCount()
{
    QSqlDatabase db;
    const QString connect_name="ConnectMain";
    if (QSqlDatabase::contains(connect_name)){
        db = QSqlDatabase::database(connect_name);
    }else{
        db = QSqlDatabase::addDatabase("QSQLITE",connect_name);
        db.setDatabaseName("Test.db");
    }

    if(db.open()){
        QSqlQuery query(db);
        int result=0;
        const QString sql=QString("SELECT COUNT(*) FROM MY_TABLE;");
        if(query.exec(sql)&&query.next()){
            result=query.value("COUNT(*)").toInt();
        }
        qDebug()<<"search count"<<result;
        db.close();
    }
}

void MainWindow::writeTest(int index, int loop, bool lock)
{
    int loop_count=0;
    QSqlDatabase db;
    const QString connect_name=QString("Connect%1").arg(index);
    if (QSqlDatabase::contains(connect_name)){
        QSqlDatabase::removeDatabase(connect_name);
    }
    db = QSqlDatabase::addDatabase("QSQLITE",connect_name);
    db.setDatabaseName("Test.db");

    if(db.open()){
        qDebug()<<"open"<<index;
        QSqlQuery query(db);
        const QString sql=QString("INSERT INTO MY_TABLE(IDX,LOOP,TIME) VALUES(?,?,?);");
        for(int i=0;i<loop;i++)
        {
            if(stopFlag)
                break;

            if(lock){
                this->mtx.lock();
            }

            if(query.prepare(sql)){
                query.bindValue(0,index);
                query.bindValue(1,i);
                query.bindValue(2,QTime::currentTime().toString("hh:mm:ss:zzz"));
                if(query.exec()){
                    loop_count++;
                    qDebug()<<"index"<<index<<"success"<<i;
                }else{
                    qDebug()<<"index"<<index<<"failed"<<i<<query.lastError();
                }
            }

            if(lock){
                this->mtx.unlock();
            }
        }
        db.close();
    }
    qDebug()<<"index"<<index<<"loop"<<loop<<"do"<<loop_count;
}

