#include <QGuiApplication>
#include <QQmlProperty>
#include <QQuickView>
#include <QQuickItem>
#include <QMetaObject>
#include <QDebug>

#include "CppObject.h"

int main(int argc, char *argv[])
{
    QCoreApplication::setAttribute(Qt::AA_EnableHighDpiScaling);

    QGuiApplication app(argc, argv);

    //可以用QQmlComponent\QQuickView\QQuickWidget的C++代码加载QML文档
    //QQuickView不能用Window做根元素
    QQuickView view(QUrl("qrc:/main.qml"));
    view.show();

    //获取到qml根对象的指针
    QObject *qmlObj=view.rootObject();

    /*文档如是说：
    应该始终使用QObject::setProperty()、QQmlProperty
    或QMetaProperty::write()来改变QML的属性值，以确保QML引擎感知属性的变化。*/

    //【1】
    //通过QObject设置属性值
    qDebug()<<"Cpp set qml property height";
    //qmlObj->setProperty("height",300);
    QQmlProperty(qmlObj,"height").write(300);
    //通过QObject获取属性值
    qDebug()<<"Cpp get qml property height"<<qmlObj->property("height");
    //任何属性都可以通过C++访问
    qDebug()<<"Cpp get qml property msg"<<qmlObj->property("msg");

    //【2】
    QQuickItem *item=qobject_cast<QQuickItem*>(qmlObj);
    //通过QQuickItem设置属性值
    qDebug()<<"Cpp set qml property width";
    item->setWidth(300);
    //通过QQuickItem获取属性值
    qDebug()<<"Cpp get qml property width"<<item->width();

    //【3】
    //通过objectName访问加载的QML对象
    //QObject::findChildren()可用于查找具有匹配objectName属性的子项
    QObject *qmlRect=qmlObj->findChild<QObject*>("rect");
    if(qmlRect){
        qDebug()<<"Cpp get rect color"<<qmlRect->property("color");
    }

    //【4】
    //调用QML方法
    QVariant val_return;  //返回值
    QVariant val_arg="GongJianBo";  //参数值
    //Q_RETURN_ARG()和Q_Arg()参数必须制定为QVariant类型
    QMetaObject::invokeMethod(qmlObj,
                              "qml_method",
                              Q_RETURN_ARG(QVariant,val_return),
                              Q_ARG(QVariant,val_arg));
    qDebug()<<"QMetaObject::invokeMethod result"<<val_return; //qml函数中返回“ok”

    //【5】
    //关联信号槽
    CppObject cppObj;
    //关联qml信号与cpp槽
    //如果信号参数为QML对象类型，信号用var参数类型，槽用QVariant类型接收
    QObject::connect(qmlObj,SIGNAL(qmlSendMsg(QString,QString)),
                     &cppObj,SLOT(cppRecvMsg(QString,QString)));
    //关联cpp信号与qml槽
    //qml中js函数参数为var类型，信号也用QVariant类型
    QObject::connect(&cppObj,SIGNAL(cppSendMsg(QVariant,QVariant)),
                     qmlObj,SLOT(qmlRecvMsg(QVariant,QVariant)));
    //此外，cpp信号也可以关联qml信号

    return app.exec();
}
