#include <QGuiApplication>
#include <QQmlApplicationEngine>
#include <QQuickWindow>

#include "AbnormalSignalModel.h"
#include "AbnormalListModel.h"

int main(int argc, char *argv[])
{
    QCoreApplication::setAttribute(Qt::AA_EnableHighDpiScaling);
    QQuickWindow::setTextRenderType(QQuickWindow::NativeTextRendering);

    QGuiApplication app(argc, argv);

    QQmlApplicationEngine engine;
    qmlRegisterType<AbnormalSignalModel>("MyModel", 1, 0, "AbnormalSignalModel");
    qmlRegisterType<AbnormalListModel>("MyModel", 1, 0, "AbnormalListModel");

    const QUrl url(QStringLiteral("qrc:/main.qml"));
    QObject::connect(&engine, &QQmlApplicationEngine::objectCreated,
                     &app, [url](QObject *obj, const QUrl &objUrl) {
        if (!obj && url == objUrl)
            QCoreApplication::exit(-1);
    }, Qt::QueuedConnection);
    engine.load(url);

    return app.exec();
}
