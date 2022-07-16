#include "Translator.h"
#include <QCoreApplication>
#include <QLocale>
#include <QMetaEnum>
#include <QtEndian>
#include <QFile>
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
    //qDebug()<<__FUNCTION__<<trans.language()<<trans_path<<QLocale::system().name();
    //可以根据翻译文件中的语言设置来设置locale
    //5.15才提供了language接口
#if (QT_VERSION < QT_VERSION_CHECK(5,15,0))
    locale = QLocale(parseLanguage(trans_path));
#else
    locale = QLocale(trans.language());
#endif
    QLocale::setDefault(locale);
    emit languageChanged();
}

QLocale Translator::getLocale() const
{
    return locale;
}

QString Translator::parseLanguage(const QString &qmpath)
{
    QString result;
    QFile file(qmpath);
    if(file.size() > 21 && file.open(QIODevice::ReadOnly))
    {
        file.read(16); //MagicLength，16个字节的标识忽略
        uchar temp[5]{}; //每个段开头个一个字节标识和四个字节长度位
        file.read((char *)temp, 5);
        quint8 tag = qFromBigEndian<quint8>(temp);
        quint32 block_len = qFromBigEndian<quint32>(temp + 1);
        //0xA7表示Language标记，为Qt5.15新增
        if(tag == 0xA7 && block_len > 0 && file.size() >= 21 + block_len)
        {
            result = file.read(block_len);
        }
        file.close();
    }
    return result;
}
