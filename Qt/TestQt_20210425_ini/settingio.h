// 开源项目中找的自定义QSettings的代码
// https://github.com/linuxdeepin/deepin-terminal
// Copyright (C) 2019 ~ 2020 Uniontech Software Technology Co.,Ltd.
// SPDX-FileCopyrightText: 2023 UnionTech Software Technology Co., Ltd.
//
// SPDX-License-Identifier: GPL-3.0-or-later

#ifndef SETTINGIO_H
#define SETTINGIO_H

#include <QObject>
#include <QSettings>
#include <QIODevice>
#include <QStringList>

class SettingIO : public QObject
{
    Q_OBJECT
public:
    explicit SettingIO(QObject *parent = nullptr);
    /**
     * @brief 读取自定义配置文件
     * @author ut001811 朱科伟
     * @param device
     * @param settingsMap
     * @return
     */
    static bool readIniFunc(QIODevice &device,  QSettings::SettingsMap &settingsMap);
    /**
     * @brief 写入自定义配置文件
     * @author ut001811 朱科伟
     * @param device
     * @param settingsMap
     * @return
     */
    static bool writeIniFunc(QIODevice &device, const QSettings::SettingsMap &settingsMap);
private:
    /**
     * @brief variant 在转换成QString
     * @author ut001811 朱科伟
     * @param v
     * @return
     */
    static QString variantToString(const QVariant &v);
    /**
     * @brief stringToVariant
     * @author ut001811 朱科伟
     * @author 将QString转换为Variant
     * @param s
     * @return
     */
    static QVariant stringToVariant(const QString &s);
    /**
     * @brief 加入特殊字符\x
     * @author ut001811 朱科伟
     * @param src
     * @return
     */
    static QByteArray escapedString(const QString &src);
    /**
     * @brief 去除特殊字符\x
     * @author ut001811 朱科伟
     * @param src
     * @return
     */
    static QString unescapedString(const QString &src);
    /**
     * @brief uincode是否需要被转换
     * @author ut001811 朱科伟
     * @param str
     * @return
     */
    static QString canTransfer(const QString &str);
    /**
     * @brief 去除unicode的标识信息 %U
     * @author ut001811 朱科伟
     * @param key
     * @param from
     * @param to
     * @param result
     * @return
     */
    static bool iniUnescapedKey(const QByteArray &key, int from, int to, QString &result);
public:
    static bool rewrite;
};

/**
 * @brief The USettings class
 * 1.conf配置文件“/”与QSettings(SLASH_REPLACE_CHAR)通过SettingIO实现转换
 * 2.QSettings(SLASH_REPLACE_CHAR)与Conmmand/Remote(/)通过USettings实现转换
 */
class USettings : protected QSettings
{
    Q_OBJECT
public:
    USettings(const QString &fileName, QObject *parent = nullptr);
    ~USettings();

    /**
     * @brief Appends prefix to the current group.
     * @author ut001121 zhangmeng
     * @param prefix
     */
    void beginGroup(const QString &prefix);
    /**
     * @brief Resets the group to what it was before the corresponding beginGroup() call.
     * @author ut001121 zhangmeng
     */
    void endGroup();

    /**
     * @brief Sets the value of setting key to value. If the key already exists, the previous value is overwritten.
     * @author ut001121 zhangmeng
     * @param key
     * @param value
     */
    void setValue(const QString &key, const QVariant &value);
    /**
     * @brief Returns the value for setting key. If the setting doesn't exist, returns defaultValue.
     * @author ut001121 zhangmeng
     * @param key
     * @param defaultValue
     * @return
     */
    QVariant value(const QString &key, const QVariant &defaultValue = QVariant()) const;

    /**
     * @brief Removes the setting key and any sub-settings of key.
     * @author ut001121 zhangmeng
     * @param key
     */
    void remove(const QString &key);
    /**
     * @brief Returns true if there exists a setting called key; returns false otherwise.
     * @author ut001121 zhangmeng
     * @param key
     * @return
     */
    bool contains(const QString &key) const;

    /**
     * @brief Returns a list of all key top-level groups that contain keys that can be read using the QSettings object.
     * @author ut001121 zhangmeng
     * @return
     */
    QStringList childGroups();

private:
    //自定义规则
    static Format g_customFormat;
};

#endif // SETTINGIO_H
