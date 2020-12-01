#include "KeysFilter.h"

KeysFilter::KeysFilter(QObject *parent)
    : QObject(parent)
{
    if(parent){
        parent->installEventFilter(this);
    }
}

void KeysFilter::setEnable(bool enable)
{
    if(this->enable!=enable){
        this->enable=enable;
        emit enableChanged();
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
    case QEvent::KeyPress:
    case QEvent::KeyRelease:
        if(enable){
            QKeyEvent *key_event=static_cast<QKeyEvent*>(event);
            if(filterKeys.contains(key_event->key())){
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
