#include "KeyboardUtil.h"

//#ifdef _WIN32
#include <Windows.h>
#include <QDebug>

KeyboardUtil::KeyboardUtil(QObject *parent) : QObject(parent)
{
}

void KeyboardUtil::checkCapslock()
{
    emit capslockChanged();
}

bool KeyboardUtil::isCapslock() const
{
    //对于该API返回值
    //如果高位为1，则按键按下；否则为0。否则，它会上升。
    //如果低位为1，则切换键。如果打开了一个键（例如CAPS LOCK键），则会对其进行切换。
    //如果低位为0，则此键处于关闭状态且不切换。
    //切换键时，键盘上的切换键指示灯（如果有）将亮起，而当取消切换键时，其指示灯将熄灭。
    return (GetKeyState(VK_CAPITAL)!=0);
}

void KeyboardUtil::attachWatch(QObject *watch)
{
    watch->installEventFilter(this);
}

bool KeyboardUtil::eventFilter(QObject *watched, QEvent *event)
{
    if(event->type()==QEvent::KeyPress||
            event->type()==QEvent::KeyRelease){
        QKeyEvent *keyevent=static_cast<QKeyEvent*>(event);
        if(keyevent->key()==Qt::Key_CapsLock){
            emit capslockChanged();
        }
    }
    return QObject::eventFilter(watched,event);
}
