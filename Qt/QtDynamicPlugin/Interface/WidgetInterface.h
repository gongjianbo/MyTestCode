#pragma once
#include <QtPlugin>
#include <QWidget>

class WidgetInterface : public QWidget
{
    Q_OBJECT
public:
    using QWidget::QWidget;

    virtual QString pluginName() const = 0;

signals:
    void pluginSignal(const QString &arg);

public slots:
    virtual void pluginSlot(const QString &arg) = 0;
};

#define WidgetInterface_iid "GongJianBo.Demo.WidgetInterface/1.0"
Q_DECLARE_INTERFACE(WidgetInterface, WidgetInterface_iid)
