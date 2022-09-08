#pragma once
#include <QObject>
#include <QJSValue>
#include <QJSEngine>
#include <QQuickItem>

class CppObject : public QObject
{
    Q_OBJECT
public:
    explicit CppObject(QObject *parent = nullptr);

    //加上Q_INVOKABLE或者声明为槽函数，就能在qml中调用这个cpp对象的函数
    Q_INVOKABLE QString cppFunc(int i);

    //通过QMetaObject::invokeMethod直接回调qml对象的函数
    Q_INVOKABLE void doInvoke(int i, QQuickItem *item);

    //通过QJSValue::call调用js函数参数
    //Qt5 QJSValue的call接口不是const，不能传const引用
    Q_INVOKABLE void doSomething(int i, QJSValue callback);
};
