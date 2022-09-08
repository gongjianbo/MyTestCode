#include "CppObject.h"
#include <QDebug>

CppObject::CppObject(QObject *parent)
    : QObject(parent)
{

}

QString CppObject::cppFunc(int i)
{
    qDebug()<<"cpp function run"<<i;
    return "cpp return value";
}

void CppObject::doInvoke(int i, QQuickItem *item)
{
    qDebug()<<"cpp doInvoke"<<i;
    QVariant val_return;  //返回值
    //Q_RETURN_ARG()和Q_Arg()参数必须制定为QVariant类型
    QMetaObject::invokeMethod(item,
                              "qmlFunc",
                              Q_RETURN_ARG(QVariant, val_return),
                              Q_ARG(QVariant, i));
    qDebug()<<"QMetaObject::invokeMethod ret:"<<val_return;
}

void CppObject::doSomething(int i, QJSValue callback)
{
    qDebug()<<"cpp doSomething"<<i;
    QJSValueList arglist;
    arglist.append(callback.engine()->toScriptValue(i));
    arglist.push_back(QJSValue(true));
    if(callback.isCallable()){
        //回调qml中的函数
        auto ret = callback.call(arglist);
        qDebug()<<"cpp callback ret:"<<ret.toString();
    }
}
