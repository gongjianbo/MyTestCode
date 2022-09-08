#include <QGuiApplication>
#include <QQmlApplicationEngine>
#include "CppObject.h"

int main(int argc, char *argv[])
{
#if QT_VERSION < QT_VERSION_CHECK(6, 0, 0)
    QCoreApplication::setAttribute(Qt::AA_EnableHighDpiScaling);
#endif
    QGuiApplication app(argc, argv);

    //注册为QML类型
    qmlRegisterType<CppObject>("Test", 1, 0, "CppObject");

    QQmlApplicationEngine engine;
    const QUrl url(QStringLiteral("qrc:/main.qml"));
    QObject::connect(&engine, &QQmlApplicationEngine::objectCreated,
                     &app, [url](QObject *obj, const QUrl &objUrl) {
        if(!obj && url == objUrl)
            QCoreApplication::exit(-1);
        if(!obj){
            return;
        }
        //通过对象树找到这个qml对象，直接执行其函数，qml对象需要设置objectName用于查找
        /*QObject *qml_obj = obj->findChild<QObject*>("qml_obj");
        if(qml_obj){
            QVariant val_return;  //返回值
            //Q_RETURN_ARG()和Q_Arg()参数必须制定为QVariant类型
            QMetaObject::invokeMethod(qml_obj,
                                      "qmlFunc",
                                      Q_RETURN_ARG(QVariant, val_return),
                                      Q_ARG(QVariant, 666));
            qDebug()<<"QMetaObject::invokeMethod ret:"<<val_return;
        }*/
    }, Qt::QueuedConnection);
    engine.load(url);

    return app.exec();
}
