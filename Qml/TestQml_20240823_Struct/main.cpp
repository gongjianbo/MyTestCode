#include <QGuiApplication>
#include <QQmlApplicationEngine>
#include "CppTool.h"

int main(int argc, char *argv[])
{
#if QT_VERSION < QT_VERSION_CHECK(6, 0, 0)
    QCoreApplication::setAttribute(Qt::AA_EnableHighDpiScaling);
#endif
    QGuiApplication app(argc, argv);

    qRegisterMetaType<QByteArray>("QByteArray");
    qRegisterMetaType<QVariant>("QVariant");
    qRegisterMetaType<QString>("QString");
    qRegisterMetaType<QVector<QImage>>("QVector<QImage>");
    qRegisterMetaType<QVector<int>>("QVector<int>");
    qRegisterMetaType<CppData>("CppData");
    qRegisterMetaType<QVector<CppData>>("QVector<CppData>");

    QQmlApplicationEngine engine;

    engine.setObjectOwnership(CppTool::getInstance(), QQmlEngine::CppOwnership);
    qmlRegisterSingletonType<CppTool>(
        "GongJianBo", 1, 0,
        "CppTool", [](QQmlEngine *, QJSEngine *){
            return CppTool::getInstance();
        });

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
