#include "WindowPlugin.h"
#include <QDebug>

WindowPlugin::WindowPlugin(QObject *parent)
    : WindowPluginInterface{parent}
{
    //Q_INIT_RESOURCE(xx.qrc);
    qDebug()<<__FUNCTION__;
}

WindowPlugin::~WindowPlugin()
{
    qDebug()<<__FUNCTION__;
}

QString WindowPlugin::name() const
{
    return QStringLiteral("I'm WindowPlugin");
}

QUrl WindowPlugin::url() const
{
    //无论是提供url还是注册为qml类型，都需要唯一性
    //为了防止和主程序qml重名，加了一个路径前缀
    return QUrl("qrc:/WindowPlugin/MyWindow.qml");
}
