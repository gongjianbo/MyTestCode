#include <QGuiApplication>
#include <QQmlApplicationEngine>
#include "MyMouseArea.h"
#include "MyMouseFilter.h"

int main(int argc, char *argv[])
{
#if QT_VERSION < QT_VERSION_CHECK(6, 0, 0)
    QCoreApplication::setAttribute(Qt::AA_EnableHighDpiScaling);
#endif
    QGuiApplication app(argc, argv);

    qmlRegisterType<MyMouseFilter>("Test", 1, 0, "MyMouseFilter");
    qmlRegisterType<MyMouseArea>("Test", 1, 0, "MyMouseArea");
    qmlRegisterRevision<QQuickMouseArea, 15>("Test", 1, 0);

    QQmlApplicationEngine engine;
    const QUrl url(QStringLiteral("qrc:/main.qml"));
    QObject::connect(
        &engine,
        &QQmlApplicationEngine::objectCreated,
        &app,
        [url](QObject *obj, const QUrl &objUrl) {
            if (!obj && url == objUrl)
                QCoreApplication::exit(-1);
        },
        Qt::QueuedConnection);
    engine.load(url);

    return app.exec();
}
