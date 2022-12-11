#pragma once
#include <QObject>
#include "PluginInterface.h"

class WindowPlugin : public WindowPluginInterface
{
    Q_OBJECT
    Q_PLUGIN_METADATA(IID "GongJianBo.Demo.WindowPluginInterface/1.0" FILE "plugin.json")
    Q_INTERFACES(WindowPluginInterface)
public:
    explicit WindowPlugin(QObject *parent = nullptr);
    ~WindowPlugin();

    QString name() const override;
    QUrl url() const override;
};
