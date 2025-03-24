// 网上找的给qsettings支持注释的demo
// https://www.cnblogs.com/cute/archive/2011/03/01/1968010.html
#pragma once
#include <QString>
#include <QVariant>

class QSettings;

class Settings
{
public:
    Settings(const QString &fileName);
    ~Settings();
    void setValue(const QString &key, const QVariant &value);
    QVariant value(const QString &key, const QVariant &defaultValue = QVariant()) const;
private:
    QSettings *d;
};

