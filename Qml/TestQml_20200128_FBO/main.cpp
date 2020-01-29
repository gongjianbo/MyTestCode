#include <QGuiApplication>
#include <QQmlApplicationEngine>

#include  "FBOItem.h"

int main(int argc, char *argv[])
{
    QCoreApplication::setAttribute(Qt::AA_EnableHighDpiScaling);

    QGuiApplication app(argc, argv);    

    //把item类型注册到qml中<Cpp类型>(qml模块名,主版本,次版本,qml类型)
    qmlRegisterType<FBOItem>("LearnOpenGL", 1, 0, "FBOItem");

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
