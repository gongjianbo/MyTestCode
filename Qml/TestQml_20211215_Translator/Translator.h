#pragma once
#include <QObject>
#include <QTranslator>
#include <QLocale>
#include <QMap>

//管理语言翻译
class Translator : public QObject
{
    Q_OBJECT
    Q_PROPERTY(Translator::Language language READ getLanguage WRITE setLanguage NOTIFY languageChanged)
    Q_PROPERTY(QLocale locale READ getLocale NOTIFY languageChanged)
public:
    enum Language {
        AnyLanguage = 0, //未设置
        ZH_CN, //中文
        EN_US //英文
    };
    Q_ENUM(Language)
private:
    explicit Translator(QObject *parent = nullptr);
public:
    ~Translator();
    static Translator *getInstance();

    Translator::Language getLanguage() const;
    void setLanguage(Translator::Language type);

    //用于一些组件属性绑定
    QLocale getLocale() const;

private:
    //QTranslator在Qt5.15才有language接口，低版本可以自己解析
    QString parseLanguage(const QString &qmpath);

signals:
    void languageChanged();

private:
    //当前语言设置
    Language lang{AnyLanguage};
    //加载翻译文件
    QTranslator trans;
    //语言枚举与language name的映射表
    QMap<Language,QString> langMap;
    //当前locale设置
    QLocale locale;
};
