#include <QGuiApplication>
#include <QQmlApplicationEngine>
#include "QSGLine.h"
#include "MyLine.h"
#include "SmoothLine.h"

int main(int argc, char *argv[])
{
#if QT_VERSION < QT_VERSION_CHECK(6, 0, 0)
    QCoreApplication::setAttribute(Qt::AA_EnableHighDpiScaling);
#endif

    QGuiApplication app(argc, argv);

    qmlRegisterType<QSGLine>("Gt.Component", 1, 0, "QSGLine");
    qmlRegisterType<MyLine>("Gt.Component", 1, 0, "MyLine");
    qmlRegisterType<SmoothLine>("Gt.Component", 1, 0, "SmoothLine");

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
