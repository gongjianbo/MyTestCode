#include "KeysFilter.h"

KeysFilter::KeysFilter(QObject *parent)
    : QObject(parent)
{
    if(parent){
        parent->installEventFilter(this);
    }
}

void KeysFilter::setEnabled(bool enable)
{
    if(enabled!=enable){
        enabled=enable;
        emit enabledChanged();
    }
}

void KeysFilter::setAcceptShortcut(bool enable)
{
    if(acceptShortcut!=enable){
        acceptShortcut=enable;
        emit acceptShortcutChanged();
    }
}

void KeysFilter::setFilterKeys(const QList<int> &filters)
{
    filterKeys=filters;
    emit filterKeysChanged();
}

bool KeysFilter::eventFilter(QObject *watched, QEvent *event)
{
    Q_UNUSED(watched)
    switch(event->type())
    {
    //对于control的space，触发了这三种keyevent
    case QEvent::ShortcutOverride:
        if(acceptShortcut)
            break;
    case QEvent::KeyPress:
    case QEvent::KeyRelease:
        if(enabled){
            QKeyEvent *key_event=static_cast<QKeyEvent*>(event);
            if(filterKeys.contains(key_event->key())){
                //accepted之后，shortcut就不再处理了
                key_event->setAccepted(true);
                return true;
            }
        }
    }
    return false;
}

KeysFilter *KeysFilter::qmlAttachedProperties(QObject *obj)
{
    return new KeysFilter(obj);
}
