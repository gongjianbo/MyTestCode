#include <QGuiApplication>
#include <QQmlApplicationEngine>
#include "FileTool.h"
#include "ClipboardTool.h"

int main(int argc, char *argv[])
{
#if QT_VERSION < QT_VERSION_CHECK(6, 0, 0)
    QCoreApplication::setAttribute(Qt::AA_EnableHighDpiScaling);
#endif
    QGuiApplication app(argc, argv);

    QQmlApplicationEngine engine;

    qmlRegisterSingletonType<FileTool>(
                "GongJianBo", 1, 0,
                "FileTool", [](QQmlEngine* qmlEngine, QJSEngine*){
        auto instance = new FileTool;
        qmlEngine->setObjectOwnership(instance, QQmlEngine::JavaScriptOwnership);
        return instance;
    });
    qmlRegisterSingletonType<ClipboardTool>(
                "GongJianBo", 1, 0,
                "ClipboardTool", [](QQmlEngine* qmlEngine, QJSEngine*){
        auto instance = new ClipboardTool;
        qmlEngine->setObjectOwnership(instance, QQmlEngine::JavaScriptOwnership);
        return instance;
    });

    const QUrl url(QStringLiteral("qrc:/main.qml"));
    QObject::connect(&engine, &QQmlApplicationEngine::objectCreated,
                     &app, [url](QObject *obj, const QUrl &objUrl) {
        if (!obj && url == objUrl)
            QCoreApplication::exit(-1);
    }, Qt::QueuedConnection);
    engine.load(url);

    return app.exec();
}
