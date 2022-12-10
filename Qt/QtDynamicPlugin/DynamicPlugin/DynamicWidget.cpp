#include "DynamicWidget.h"
#include <QVBoxLayout>
#include <QPushButton>
#include <QDebug>

DynamicWidget::DynamicWidget(QWidget *parent)
    : WidgetInterface{parent}
{
    QVBoxLayout *layout = new QVBoxLayout(this);
    QPushButton *btn = new QPushButton("Plugin", this);
    connect(btn, &QPushButton::clicked, [=](){
        emit pluginSignal("Plugin Emit Signal");
    });
    layout->addWidget(btn);
    layout->setMargin(0);
}

QString DynamicWidget::pluginName() const
{
    return "I'm DynamicWidget";
}

void DynamicWidget::pluginSlot(const QString &arg)
{
    qDebug()<<"Plugin Recv"<<arg;
}
