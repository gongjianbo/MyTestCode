#include "MyTabPage.h"

#include <QDebug>
#include <QLabel>

//用于累加计算唯一id
int global_id=0;
//用于保存当前存在的page实例的id
QList<int> active_id;

MyTabPage::MyTabPage(QWidget *parent)
    : QWidget(parent)
{
    id=global_id++;

    setWindowTitle(QString::number(id));
    //用来显示当前page的唯一id
    QLabel *label=new QLabel("ID:"+QString::number(id),this);
    label->setGeometry(10,10,100,30);

    active_id.push_back(id);
    qDebug()<<id<<"init"<<active_id;
}

MyTabPage::~MyTabPage()
{
    active_id.removeOne(id);
    qDebug()<<id<<"free"<<active_id;
}
