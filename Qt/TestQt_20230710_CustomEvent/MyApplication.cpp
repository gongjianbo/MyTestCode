#include "MyApplication.h"
#include "CustomEvent.h"
#include <private/qobject_p.h>
#include <private/qthread_p.h>
#include <QApplication>
#include <QDebug>

MyApplication::MyApplication(int &argc, char **argv, int flags)
    : QApplication{argc, argv, flags}
{

}

bool MyApplication::compressEvent(QEvent *event, QObject *receiver, QPostEventList *postedEvents)
{
    // 操作事件队列的相关类和接口需要引入 需要core-private
    if (event->type() == CustomEvent::IsCustom) {
        int counter = 0;
        QPostEventList::const_iterator it = postedEvents->constBegin();
        for (; it != postedEvents->constEnd(); ++it)
        {
            const QPostEvent &cur = *it;
            if (cur.receiver != receiver || cur.event == nullptr || cur.event->type() != event->type())
                continue;
            if (cur.event->type() == CustomEvent::IsCustom) {
                // 事件计数，如果只保留一个事件可以直接 delete 并返回
                counter++;
            } else {
                continue;
            }
            // 允许队列缓存的事件数
            if (counter >= 2) {
                // 把后面 post 的释放掉并返回 true，就完成了合并
                delete event;
                return true;
            }
        }
    }
    return QApplication::compressEvent(event, receiver, postedEvents);
}
