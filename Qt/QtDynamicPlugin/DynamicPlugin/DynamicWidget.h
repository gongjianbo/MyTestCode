#pragma once
#include <QWidget>
#include "WidgetInterface.h"

class DynamicWidget : public WidgetInterface
{
    Q_OBJECT
    Q_PLUGIN_METADATA(IID "GongJianBo.Demo.WidgetInterface/1.0" FILE "dynamicwidget.json")
    Q_INTERFACES(WidgetInterface)
public:
    explicit DynamicWidget(QWidget *parent = nullptr);

     QString pluginName() const override;

public slots:
     void pluginSlot(const QString &arg) override;
};
