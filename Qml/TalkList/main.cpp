#include <QGuiApplication>
#include <QQmlApplicationEngine>
#include <QDebug>

#include "TalkListDefine.h"
#include "TalkListModel.h"

#include <string.h>
#if (_WIN32||_WIN64)
#define FOLDER_SEP '\\'
#else
#define FOLDER_SEP '/'
#endif
static const char *FileBaseName(const char *filename)
{
    //从右往左找字符，成功则返回该符号偏移(所以+1去掉该字符)
    //失败返回原名
    const char *rv = std::strrchr(filename,FOLDER_SEP);
    return rv != nullptr ? rv+1 : filename;
}

int main(int argc, char *argv[])
{
    qDebug()<<FileBaseName(__FILE__)<<__FUNCTION__<<__LINE__;
    QCoreApplication::setAttribute(Qt::AA_EnableHighDpiScaling);

    QGuiApplication app(argc, argv);

    qmlRegisterType<TalkData>("TalkModel",1,0,"TalkData");
    qmlRegisterType<TalkListModel>("TalkModel",1,0,"TalkListModel");
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
