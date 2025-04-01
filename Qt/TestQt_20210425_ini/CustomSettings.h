#pragma once
#include <QSettings>
#include <QIODevice>
#include <QStringList>
#include <QHash>

/**
 * @brief 自定义ini读写
 * @author 龚建波
 * @date 2025-03-21
 * @details
 * 因为读写接口没法传入额外的信息，如QSetting设置的codec编码信息，只能根据设置注册不同的QSettings::Format
 * 可以不判断设置的codec，用固定的编码
 * 也可以根据文件bom判断格式，不支持的就用默认utf8读写，但是没法获取文件名不能保证读写编码一致
 */
class CustomSettingsIO
{
public:
    // QSettings::registerFormat注册用到的读取回调
    static bool readIniFunc(QIODevice &device,  QSettings::SettingsMap &settingsMap);
    // QSettings::registerFormat注册用到的写入回调
    static bool writeIniFunc(QIODevice &device, const QSettings::SettingsMap &settingsMap);

private:
    // 获取文件编码
    static QTextCodec* parseCodec(QIODevice &device);
    // QVariant转QString
    static QString variantToString(const QVariant &v);
    // QString转QVariant
    static QVariant stringToVariant(const QString &s);
    // stringToVariant中分割Qt类型转义字符串
    static QStringList splitArgs(const QString &s, int idx);

private:
    // 写文件用固定的编码
    static QTextCodec *iniCodec;
};

/**
 * @brief 自定义QSettings支持需要的编码
 * @author 龚建波
 * @date 2025-03-21
 * @details
 * 借鉴：qt-everywhere-src-5.15.2/qtbase/src/corelib/io/qsettings.h
 * 借鉴：https://github.com/linuxdeepin/deepin-terminal
 * @note
 * QSettings有缓存，如果先用A编码读取，那再用B编码打开依然会用到之前的数据，文件修改后才会重新读取
 * 不同QSettings::Format不能混用操作同一个文件
 */
class CustomSettings : public QSettings
{
public:
    CustomSettings(const QString &fileName, QObject *parent = nullptr);
    ~CustomSettings();

private:
    // 自定义规则
    static QSettings::Format customIniFormat;
};
