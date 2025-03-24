#pragma once
#include <QObject>
#include <QVariant>
#include <QList>

namespace Tool {

/**
 * @brief 简易的ini读写类
 * @author 龚建波
 * @date 2021-04-25
 * @details
 * 用于替代QSettings读写ini文件
 * 因为QSettings不支持中文key/groups/;注释等
 * @note
 * 不支持并发操作
 * 无缓存
 * 无异常处理
 * 测试编码为utf-8
 */
class EasyIni : public QObject
{
    Q_OBJECT
public:
    // 操作模式
    enum Mode
    {
        ReadOnly,
        WriteOnly,
        ReadWrite
    };
public:
    explicit EasyIni(const QString &filepath,
                     EasyIni::Mode mode = EasyIni::ReadWrite,
                     QObject *parent = nullptr);
    ~EasyIni();

    // 打印ini信息
    void dumpInfo() const;
    // 分组列表
    QStringList groups() const;
    // 分组内所有key
    QStringList keys(const QString &group) const;
    // 获取值
    QVariant value(const QString &group, const QString &key) const;
    // 设置值，group或key不存在则创建
    void setValue(const QString &group, const QString &key, const QVariant &value);
    // 保存
    void save();
    // 加载
    void load(const QString &filepath = QString());

private:
    // 判断当前路径是否可写
    bool isWritable() const;
    // 解析ini文件
    void parse(const QString &filepath);
    // variant转str copy from qsettings
    static QString variantToString(const QVariant &v);
    // str转variant copy from qsettings
    static QVariant stringToVariant(const QString &s);
    // stringToVariant中用到的分割 copy from qsettings
    static QStringList splitArgs(const QString &s, int idx);

private:
    // ini文件路径
    QString inipath;
    // 操作模式
    EasyIni::Mode iniMode = EasyIni::ReadWrite;

    // 【】数据结构
    // 一行的数据
    struct IniRow{
        // 是否有效，无效则仅保存在写入时恢复原格式
        bool isValid = false;
        // 行key
        QString key;
        // 行value
        QVariant value;
    };
    // 一个分组的数据
    struct IniGroup{
        // 是否为默认初始分组
        bool isHead = false;
        // 分组名
        QString group;
        // 分组行列表
        QList<IniRow> rows;
        // group key列表
        // 无效行为空
        QList<QString> allkeys;
    };
    // ini分组列表
    struct IniData{
        // 标记数据改变了，在析构时写入文件
        // 或者主动写入文件
        bool change = false;
        // 分组数据
        QList<IniGroup> datas;
        // group name列表
        // head和无效为空
        QList<QString> allgroups;
    };

    // ini数据
    IniData iniData;
};

}
