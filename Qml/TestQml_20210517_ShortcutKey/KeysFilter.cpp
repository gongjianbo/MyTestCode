#include "KeysFilter.h"
#include <QGuiApplication>
#include <QKeyEvent>
//#include <QDebug>

KeysFilter::KeysFilter(QObject *parent)
    : QObject(parent)
{

}

bool KeysFilter::eventFilter(QObject *watched, QEvent *event)
{
    Q_UNUSED(watched)
    const int &&e_type = event->type();
    switch(e_type)
    {
    case QEvent::ShortcutOverride:
        if(filterType & FilterType::ShortcutOverride) goto Tab_KeyProcess;
        break;
    case QEvent::KeyPress:
        if(filterType & FilterType::KeyPress) goto Tab_KeyProcess;
        break;
    case QEvent::KeyRelease:
        if(filterType & FilterType::KeyRelease) goto Tab_KeyProcess;
        break;
    }
    //if(static_cast<QKeyEvent*>(event)){
    //    qDebug()<<"not filter keys."<<event;
    //}
    return false;

    Tab_KeyProcess:
    {
        QKeyEvent *key_event = static_cast<QKeyEvent*>(event);
        //qDebug()<<"key"<<key_event<<filterKeys.contains(key_event->key());
        if(key_event && filterKeys.contains(key_event->key())){
            //accepted之后，shortcut就不再处理了
            key_event->setAccepted(true);

            //下面的逻辑用不到，目前只把快捷键过滤
            if(acceptAutoRepeat || !key_event->isAutoRepeat()){
                if(e_type == QEvent::KeyPress){
                    emit pressed(key_event->key());
                }else if(e_type == QEvent::KeyRelease){
                    emit released(key_event->key());
                }
            }
            return true;
        }
    }
    return false;
}

void KeysFilter::setEnabled(bool enable)
{
    if(enabled != enable){
        if(enable){
            qApp->installEventFilter(this);
        }else{
            qApp->removeEventFilter(this);
        }

        enabled = enable;
        emit enabledChanged();
    }
}

void KeysFilter::setAcceptAutoRepeat(bool accept)
{
    if(acceptAutoRepeat != accept){
        acceptAutoRepeat = accept;
        emit acceptAutoRepeatChanged();
    }
}

void KeysFilter::setFilterType(KeysFilter::FilterType type)
{
    if(filterType != type){
        filterType = type;
        emit filterTypeChanged();
    }
}

void KeysFilter::setFilterKeys(const QList<int> &keys)
{
    filterKeys = keys;
    emit filterKeysChanged();
}
