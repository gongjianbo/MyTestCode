#include <QGuiApplication>
#include <QQmlApplicationEngine>
#include "MyClass.h"
#include <QDebug>

int main(int argc, char *argv[])
{
#if QT_VERSION < QT_VERSION_CHECK(6, 0, 0)
    QCoreApplication::setAttribute(Qt::AA_EnableHighDpiScaling);
#endif

    qDebug()<<MyClass::hello();

#ifdef IS_64_BIT_ARCH
    qDebug()<<"64 bit arch";
#elif IS_32_BIT_ARCH
    qDebug()<<"32 bit arch";
#endif

#if defined(__i386) || defined(__i386__) || defined(_M_IX86)
    qDebug()<<"32 bit arch";
#elif defined(__x86_64) || defined(__x86_64__) || defined(__amd64) || defined(_M_X64)
    qDebug()<<"64 bit arch";
#endif

    QGuiApplication app(argc, argv);

    QQmlApplicationEngine engine;
    const QUrl url(QStringLiteral("qrc:/main.qml"));
    QObject::connect(&engine, &QQmlApplicationEngine::objectCreated,
                     &app, [url](QObject *obj, const QUrl &objUrl) {
        if (!obj && url == objUrl)
            QCoreApplication::exit(-1);
    }, Qt::QueuedConnection);
    engine.load(url);

    return app.exec();
}
