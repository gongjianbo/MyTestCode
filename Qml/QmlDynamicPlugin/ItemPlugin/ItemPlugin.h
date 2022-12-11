#pragma once
#include <QObject>
#include "PluginInterface.h"

class ItemPlugin : public ItemPluginInterface
{
    Q_OBJECT
    Q_PLUGIN_METADATA(IID "GongJianBo.Demo.ItemPluginInterface/1.0" FILE "plugin.json")
    Q_INTERFACES(ItemPluginInterface)
public:
    explicit ItemPlugin(QObject *parent = nullptr);
    ~ItemPlugin();

    QString name() const override;
    QUrl url() const override;
};
