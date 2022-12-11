#include <QGuiApplication>
#include <QQmlApplicationEngine>
#include <QQmlContext>
#include <QPluginLoader>
#include <QDir>
#include <QUrl>
#include <QDebug>
#include "PluginInterface.h"

QList<QUrl> loadItemUrls(const QString &dirPath)
{
    QList<QUrl> url_list;
    QDir plugin_dir(dirPath);
    const auto entry_list = plugin_dir.entryList(QStringList()<<"*.dll");
    for (const QString &file_name : entry_list) {
        qDebug()<<file_name<<plugin_dir.absoluteFilePath(file_name);
        QPluginLoader loader(plugin_dir.absoluteFilePath(file_name));
        //所以plugin至少得是QObject的子类
        QObject *plugin = loader.instance();
        if (plugin) {
            ItemPluginInterface *pi = qobject_cast<ItemPluginInterface*>(plugin);
            if (pi) {
                qDebug()<<"Load plugin"<<pi->name();
                url_list<<pi->url();
            }
        }
    }
    return url_list;
}

QList<QUrl> loadWindowUrls(const QString &dirPath)
{
    QList<QUrl> url_list;
    QDir plugin_dir(dirPath);
    const auto entry_list = plugin_dir.entryList(QStringList()<<"*.dll");
    for (const QString &file_name : entry_list) {
        qDebug()<<file_name<<plugin_dir.absoluteFilePath(file_name);
        QPluginLoader loader(plugin_dir.absoluteFilePath(file_name));
        //所以plugin至少得是QObject的子类
        QObject *plugin = loader.instance();
        if (plugin) {
            WindowPluginInterface *pi = qobject_cast<WindowPluginInterface*>(plugin);
            if (pi) {
                qDebug()<<"Load plugin"<<pi->name();
                url_list<<pi->url();
            } else {
                loader.unload();
            }
        }
    }
    return url_list;
}

int main(int argc, char *argv[])
{
#if QT_VERSION < QT_VERSION_CHECK(6, 0, 0)
    QCoreApplication::setAttribute(Qt::AA_EnableHighDpiScaling);
#endif
    QGuiApplication app(argc, argv);

    QQmlApplicationEngine engine;

    qRegisterMetaType<QList<QUrl>>("QList<QUrl>");

    QString item_path = QCoreApplication::applicationDirPath() + "/itemplugins";
    QList<QUrl> item_list = loadItemUrls(item_path);
    engine.rootContext()->setContextProperty("itemUrls", QVariant::fromValue(item_list));

    QString window_path = QCoreApplication::applicationDirPath() + "/windowplugins";
    QList<QUrl> window_list = loadWindowUrls(window_path);
    engine.rootContext()->setContextProperty("windowUrls", QVariant::fromValue(window_list));

    const QUrl url(QStringLiteral("qrc:/main.qml"));
    QObject::connect(&engine, &QQmlApplicationEngine::objectCreated,
                     &app, [url](QObject *obj, const QUrl &objUrl) {
        if (!obj && url == objUrl)
            QCoreApplication::exit(-1);
    }, Qt::QueuedConnection);
    engine.load(url);

    return app.exec();
}
