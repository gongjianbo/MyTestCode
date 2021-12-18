#pragma once
#include <QObject>
#include <QTranslator>
#include <QMap>

//管理语言翻译
class Translator : public QObject
{
    Q_OBJECT
    Q_PROPERTY(Translator::Language language READ getLanguage WRITE setLanguage NOTIFY languageChanged)
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

signals:
    void languageChanged();

private:
    //当前语言设置
    Language lang{AnyLanguage};
    //加载翻译文件
    QTranslator trans;
    //语言枚举与language name的映射表
    QMap<Language,QString> langMap;
};

