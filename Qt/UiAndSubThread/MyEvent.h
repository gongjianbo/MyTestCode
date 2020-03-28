#ifndef MYEVENT_H
#define MYEVENT_H

#include <QObject>
#include <QEvent>

class MyEvent : public QEvent
{
public:
    static const QEvent::Type eventType;
    explicit MyEvent(const QString &str);
    ~MyEvent();

    const QString message;
};

#endif // MYEVENT_H
