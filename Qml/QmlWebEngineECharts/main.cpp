#include <QGuiApplication>
#include <QQmlApplicationEngine>
#include <qtwebengineglobal.h>

int main(int argc, char *argv[])
{
    QCoreApplication::setAttribute(Qt::AA_EnableHighDpiScaling);

    // 因为5.13的在线文档没了，所以我没法判断，请自己查下文档
#if (QT_VERSION >= QT_VERSION_CHECK(5,14,0))
    QCoreApplication::setAttribute(Qt::AA_ShareOpenGLContexts);
    // 有些情况可能需要soft opengl
    //QCoreApplication::setAttribute(Qt::AA_UseSoftwareOpenGL);
    // webengine须初始化，5.12放在app构造后，5.15放在构造前
    QtWebEngine::initialize();
    QGuiApplication app(argc, argv);
#else
    // 5.12提示Attribute Qt::AA_ShareOpenGLContexts must be set before QCoreApplication is created
    QCoreApplication::setAttribute(Qt::AA_ShareOpenGLContexts);
    // 有些情况可能需要soft opengl
    //QCoreApplication::setAttribute(Qt::AA_UseSoftwareOpenGL);
    // webengine须初始化，5.12放在app构造后，5.15放在构造前
    QGuiApplication app(argc, argv);
    QtWebEngine::initialize();
#endif

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
