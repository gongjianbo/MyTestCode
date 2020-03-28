#include "MyEvent.h"

#include <QDebug>

//注册事件类型
const QEvent::Type MyEvent::eventType=static_cast<QEvent::Type>(QEvent::registerEventType(QEvent::User+1));

MyEvent::MyEvent(const QString &str)
    :QEvent(eventType)
    ,message(str)
{
    qDebug()<<"event new";
}

MyEvent::~MyEvent()
{
    qDebug()<<"event delete";
}
