#pragma once

#include <QString>
#include <QVariant>

class QSettings;

//网上的给qsettings支持注释的demo
class Settings
{
public:
    Settings(const QString & fileName);
    ~Settings();
    void setValue ( const QString & key, const QVariant & value );
    QVariant value ( const QString & key, const QVariant & defaultValue = QVariant() ) const;
private:
    QSettings *d;
};

