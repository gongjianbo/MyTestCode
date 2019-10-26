#include <QGuiApplication>
#include <QQmlApplicationEngine>
#include <QQuickWindow>

#include "LoginDBOperate.h"
#include "KeyboardUtil.h"

int main(int argc, char *argv[])
{
    QCoreApplication::setAttribute(Qt::AA_EnableHighDpiScaling);
    QQuickWindow::setTextRenderType(QQuickWindow::NativeTextRendering);

    QGuiApplication app(argc, argv);

    qmlRegisterType<LoginDBOperate>("DBOperate", 1, 0, "LoginDBOperate");
    qmlRegisterType<KeyboardUtil>("Tools", 1, 0, "KeyboardUtil");

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
