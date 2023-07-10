#include "CustomEvent.h"
#include <QDebug>

CustomEvent::CustomEvent(const QString &msg)
    : QEvent{static_cast<QEvent::Type>(IsCustom)}, mMessage{msg}
{
    qDebug() << __FUNCTION__ << mMessage;
}

CustomEvent::~CustomEvent()
{
    qDebug() << __FUNCTION__ << mMessage;
}

const QString &CustomEvent::message() const
{
    return mMessage;
}
