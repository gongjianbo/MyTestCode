#include <QGuiApplication>
#include <QQmlApplicationEngine>
#include "DataSource.h"
#include "DataModel.h"

int main(int argc, char *argv[])
{
#if QT_VERSION < QT_VERSION_CHECK(6, 0, 0)
    QCoreApplication::setAttribute(Qt::AA_EnableHighDpiScaling);
#endif
    QGuiApplication app(argc, argv);

    //初始化填充一些测试数据
    auto source = DataSource::getInstance();
    source->appendItem("gong", 1992);
    source->appendItem("jian", 101);
    source->appendItem("bo", 10001);
    qmlRegisterType<DataModel>("Test", 1, 0, "DataModel");

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
