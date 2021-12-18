#include "Translator.h"
#include <QCoreApplication>
#include <QLocale>
#include <QMetaEnum>
#include <QDebug>

Translator::Translator(QObject *parent)
    : QObject{parent}
{
    langMap = {
        {ZH_CN,"zh_CN"},
        {EN_US,"en_US"}
    };
    qApp->installTranslator(&trans);
}

Translator::~Translator()
{
}

Translator *Translator::getInstance()
{
    static Translator instance;
    return &instance;
}

Translator::Language Translator::getLanguage() const
{
    return lang;
}

void Translator::setLanguage(Translator::Language type)
{
    if(type == lang || !langMap.contains(type))
        return;
    lang = type;
    //枚举名对应文件名，便于查找对应文件
    //但是对于可扩展的翻译，可以遍历目录下的文件，通过文件名来切换
    QString lang_str = langMap.value(type);
    QString trans_path = QString("%1/trans_%2.qm")
            .arg(qApp->applicationDirPath())
            .arg(lang_str);

    trans.load(trans_path);
    //qDebug()<<QLocale("zh_CN")<<QLocale("zh_TW");
    qDebug()<<__FUNCTION__<<trans.language()<<trans_path<<QLocale::system().name();
    //可以根据翻译文件中的语言设置来设置locale
    locale = QLocale(trans.language());
    QLocale::setDefault(locale);
    emit languageChanged();
}

QLocale Translator::getLocale() const
{
return QLocale();
}
