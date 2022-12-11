#include "ItemPlugin.h"
#include <QDebug>

ItemPlugin::ItemPlugin(QObject *parent)
    : ItemPluginInterface{parent}
{
    //Q_INIT_RESOURCE(xx.qrc);
    qDebug()<<__FUNCTION__;
}

ItemPlugin::~ItemPlugin()
{
    qDebug()<<__FUNCTION__;
}

QString ItemPlugin::name() const
{
    return QStringLiteral("I'm ItemPlugin");
}

QUrl ItemPlugin::url() const
{
    //无论是提供url还是注册为qml类型，都需要唯一性
    //为了防止和主程序qml重名，加了一个路径前缀
    return QUrl("qrc:/ItemPlugin/MyRect.qml");
}
