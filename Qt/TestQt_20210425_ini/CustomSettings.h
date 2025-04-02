#pragma once
#include <QSettings>
#include <QIODevice>
#include <QStringList>
#include <QHash>

/**
 * @brief 自定义ini读写
 * @author 龚建波
 * @date 2025-03-21
 * @note
 * 1.QSttings内部的读写接口map key是QSettingsKey类型，可以保存原始的顺序，但是公有回调接口没有
 * 2.读写接口只有map来传递数据，可以把文件编码/key顺序等信息存在map中，在回写时使用
 */
class CustomSettingsIO
{
public:
    // QSettings::registerFormat注册用到的读取回调
    static bool readIniFunc(QIODevice &device, QSettings::SettingsMap &settingsMap);
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
 * 1.QSettings有缓存，如果先用A编码读取，那再用B编码打开依然会用到之前的数据，文件修改后才会重新读取
 * 2.不同QSettings::Format不能混用操作同一个文件
 * 3.要支持section带/字符，参考deepin-terminal项目
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
