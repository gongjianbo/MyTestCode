#pragma once
#include <QtPlugin>
#include <QUrl>

//插件接口
class ItemPluginInterface : public QObject
{
    Q_OBJECT
public:
    using QObject::QObject;

    //获取插件名称
    virtual QString name() const = 0;
    //获取插件qml文件的url
    virtual QUrl url() const = 0;
};

#define ItemPluginInterface_iid "GongJianBo.Demo.ItemPluginInterface/1.0"
Q_DECLARE_INTERFACE(ItemPluginInterface, ItemPluginInterface_iid)

//插件接口
class WindowPluginInterface : public QObject
{
    Q_OBJECT
public:
    using QObject::QObject;

    //不同的插件可以提供不同的接口，这里只是为了演示
    //获取插件名称
    virtual QString name() const = 0;
    //获取插件qml文件的url
    virtual QUrl url() const = 0;
};

#define WindowPluginInterface_iid "GongJianBo.Demo.WindowPluginInterface/1.0"
Q_DECLARE_INTERFACE(WindowPluginInterface, WindowPluginInterface_iid)
