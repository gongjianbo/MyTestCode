#include "EasyIni.h"
#include <QTextStream>
#include <QDataStream>
#include <QTemporaryFile>
#include <QFileInfo>
#include <QFile>
#include <QDir>
#include <QRect>
#include <QSize>
#include <QDebug>

using namespace Tool;

EasyIni::EasyIni(const QString &filepath, EasyIni::Mode mode, QObject *parent)
    : QObject(parent), inipath(filepath), iniMode(mode)
{
    load();
}

EasyIni::~EasyIni()
{
    save();
}

void EasyIni::dumpInfo() const
{
    qDebug()<<"[dump ini info]";
    for(auto agroup:iniData.datas)
    {
        if(!agroup.isHead)
            qDebug()<<"[group]"<<agroup.group;
        for(auto arow:agroup.rows)
        {
            if(arow.isValid){
                qDebug()<<"\t[key]"<<arow.key<<"[value]"<<arow.value;
            }else{
                qDebug()<<"\t[unknown]"<<arow.key;
            }
        }
    }
    //qDebug()<<iniData.allgroups;
}

QStringList EasyIni::groups() const
{
    auto result = iniData.allgroups;
    result.removeAll(QString());
    return result;
}

QStringList EasyIni::keys(const QString &group) const
{
    QStringList key_list;
    int group_index = iniData.allgroups.indexOf(group);
    if(group.isEmpty()||group_index<0)
        return key_list;
    auto result = iniData.datas.at(group_index).allkeys;
    result.removeAll(QString());
    return result;
}

QVariant EasyIni::value(const QString &group, const QString &key) const
{
    int group_index = iniData.allgroups.indexOf(group);
    if(group.isEmpty()||key.isEmpty()||group_index<0)
        return QVariant();

    int key_index = iniData.datas.at(group_index).allkeys.indexOf(key);
    if(key_index<0)
        return QVariant();

    return iniData.datas.at(group_index).rows.at(key_index).value;
}

void EasyIni::setValue(const QString &group, const QString &key, const QVariant &value)
{
    if(group.isEmpty()||key.isEmpty())
        return;
    //如果是不存在的分组则创建
    int group_index = iniData.allgroups.indexOf(group);
    if(group_index<0){
        IniGroup ini_group;
        ini_group.isHead = false;
        ini_group.group = group;
        iniData.datas.push_back(ini_group);
        iniData.allgroups.push_back(group);
        group_index = iniData.allgroups.count()-1;
    }
    //如果不存在key则创建
    int key_index = iniData.datas.at(group_index).allkeys.indexOf(key);
    if(key_index<0){
        IniRow ini_row;
        ini_row.isValid = true;
        ini_row.key = key;
        //ini_row.value = value;
        iniData.datas[group_index].rows.push_back(ini_row);
        iniData.datas[group_index].allkeys.push_back(key);
        key_index = iniData.datas.at(group_index).allkeys.count()-1;
    }
    //插入这里有点疑问，就是
    iniData.datas[group_index].rows[key_index].value = value;
    //标记为已修改状态，在save时判断
    iniData.change = true;
}

void EasyIni::save()
{
    //只读
    if(EasyIni::ReadOnly == iniMode)
        return;

    if(iniData.change && isWritable()){
        qDebug()<<"save ini"<<inipath;
        QFile file(inipath);
        //QIODevice::Text on windows endl=\r\n
        if(file.open(QIODevice::WriteOnly|QIODevice::Truncate|QIODevice::Text))
        {
            QTextStream t_s(&file);
            t_s.setCodec("utf-8");
            for(auto agroup:iniData.datas)
            {
                if(!agroup.isHead){
                    t_s<<'['<<agroup.group<<']'<<endl;
                }

                for(auto arow:agroup.rows)
                {
                    if(arow.isValid){
                        t_s<<arow.key<<'='<<variantToString(arow.value)<<endl;
                    }else{
                        t_s<<arow.key<<endl;
                    }
                }
            }
            file.close();
        }
    }
}

void EasyIni::load(const QString &filepath)
{
    //保存当前的
    save();

    iniData = IniData{ };
    const QString path = filepath.isEmpty()?inipath:filepath;
    inipath = path;

    QFile file(path);
    if((EasyIni::WriteOnly == iniMode) ||
            !file.exists() ||
            !file.open(QIODevice::ReadOnly|QIODevice::Text)){
        return;
    }

    qDebug()<<"load ini"<<inipath;

    IniData ini_data;
    IniGroup ini_group;
    IniRow ini_row;
    QTextStream t_s(&file);
    t_s.setCodec("utf-8");
    //qDebug()<<"all"<<t_s.readAll();

    //默认分组
    ini_group.isHead=true;
    while(!t_s.atEnd()){
        //去掉首尾空格
        QString line_data=t_s.readLine().trimmed();
        //qDebug()<<"-- line:"<<line_data<<line_data.length();

        if(line_data.count()<3||
                line_data[0]==';'||
                line_data[0]=='#'){
            //长度小于三无效: a=0 or [a] 注释暂不考虑
            ini_row.isValid=false;
            ini_row.key=line_data;
            ini_row.value.clear();

            ini_group.rows.push_back(ini_row);
            ini_group.allkeys.push_back(QString());
            //qDebug()<<"invalid"<<line_data;
        }else if(line_data[0]=='['&&
                 line_data[line_data.length()-1]==']'){
            //分组
            ini_data.datas.push_back(ini_group);
            ini_data.allgroups.push_back(ini_group.group);

            ini_group.isHead=false;
            ini_group.group=line_data.mid(1,line_data.count()-2);

            ini_group.rows.clear();
            ini_group.allkeys.clear();
            //qDebug()<<"group"<<ini_group.group;
        }else{
            ini_row.isValid=false;
            int split_index=line_data.indexOf('=');
            if(split_index>0&&split_index<line_data.length()-1){
                QString key=line_data.mid(0,split_index).trimmed();
                QString value=line_data.mid(split_index+1,line_data.length()-split_index-1).trimmed();
                if(!key.isEmpty()&&!value.isEmpty()){
                    ini_row.isValid=true;
                    ini_row.key=key;
                    ini_row.value=stringToVariant(value);

                    ini_group.rows.push_back(ini_row);
                    ini_group.allkeys.push_back(key);
                    //qDebug()<<"key-value"<<key<<value;
                }
            }
            if(!ini_row.isValid){
                ini_row.key=line_data;
                ini_row.value.clear();

                ini_group.rows.push_back(ini_row);
                ini_group.allkeys.push_back(QString());
                //qDebug()<<"invalid"<<line_data;
            }
        }
    }
    file.close();

    //最后一个分组的数据
    if(ini_group.isHead||(ini_data.allgroups.count()>0&&ini_group.isHead!=true)){
        ini_data.datas.push_back(ini_group);
        ini_data.allgroups.push_back(ini_group.group);
    }

    iniData = ini_data;
}

bool EasyIni::isWritable() const
{
    bool result=false;
    QFileInfo fileinfo(inipath);
    if(fileinfo.exists()){
        QFile file(inipath);
        result=file.open(QFile::ReadWrite);
        file.close();
    }else{
        QDir dir(fileinfo.absolutePath());
        if(dir.exists()||dir.mkpath(dir.absolutePath())){
            //临时文件
            QTemporaryFile file(inipath);
            result=file.open();
            file.close();
        }
    }
    return result;
}

QString EasyIni::variantToString(const QVariant &v)
{
    QString result;

    switch (v.type()) {
    case QVariant::Invalid:
        result = QLatin1String("@Invalid()");
        break;

    case QVariant::ByteArray: {
        QByteArray a = v.toByteArray();
        result = QLatin1String("@ByteArray(")
                + QLatin1String(a.constData(), a.size())
                + QLatin1Char(')');
        break;
    }

    case QVariant::String:
    case QVariant::LongLong:
    case QVariant::ULongLong:
    case QVariant::Int:
    case QVariant::UInt:
    case QVariant::Bool:
    case QVariant::Double:
    case QVariant::KeySequence: {
        result = v.toString();
        if (result.contains(QChar::Null))
            result = QLatin1String("@String(") + result + QLatin1Char(')');
        else if (result.startsWith(QLatin1Char('@')))
            result.prepend(QLatin1Char('@'));
        break;
    }
#ifndef QT_NO_GEOM_VARIANT
    case QVariant::Rect: {
        QRect r = qvariant_cast<QRect>(v);
        result = QString::asprintf("@Rect(%d %d %d %d)", r.x(), r.y(), r.width(), r.height());
        break;
    }
    case QVariant::Size: {
        QSize s = qvariant_cast<QSize>(v);
        result = QString::asprintf("@Size(%d %d)", s.width(), s.height());
        break;
    }
    case QVariant::Point: {
        QPoint p = qvariant_cast<QPoint>(v);
        result = QString::asprintf("@Point(%d %d)", p.x(), p.y());
        break;
    }
#endif // !QT_NO_GEOM_VARIANT

    default: {
#ifndef QT_NO_DATASTREAM
        QDataStream::Version version;
        const char *typeSpec;
        if (v.type() == QVariant::DateTime) {
            version = QDataStream::Qt_5_6;
            typeSpec = "@DateTime(";
        } else {
            version = QDataStream::Qt_4_0;
            typeSpec = "@Variant(";
        }
        QByteArray a;
        {
            QDataStream s(&a, QIODevice::WriteOnly);
            s.setVersion(version);
            s << v;
        }

        result = QLatin1String(typeSpec)
                + QLatin1String(a.constData(), a.size())
                + QLatin1Char(')');
#else
        Q_ASSERT(!"stringToVariant: Cannot save custom types without QDataStream support");
#endif
        break;
    }
    }

    return result;
}

QVariant EasyIni::stringToVariant(const QString &s)
{
    if (s.startsWith(QLatin1Char('@'))) {
        if (s.endsWith(QLatin1Char(')'))) {
            if (s.startsWith(QLatin1String("@ByteArray("))) {
                return QVariant(s.midRef(11, s.size() - 12).toLatin1());
            } else if (s.startsWith(QLatin1String("@String("))) {
                return QVariant(s.midRef(8, s.size() - 9).toString());
            } else if (s.startsWith(QLatin1String("@Variant("))
                       || s.startsWith(QLatin1String("@DateTime("))) {
#ifndef QT_NO_DATASTREAM
                QDataStream::Version version;
                int offset;
                if (s.at(1) == QLatin1Char('D')) {
                    version = QDataStream::Qt_5_6;
                    offset = 10;
                } else {
                    version = QDataStream::Qt_4_0;
                    offset = 9;
                }
                QByteArray a = s.midRef(offset).toLatin1();
                QDataStream stream(&a, QIODevice::ReadOnly);
                stream.setVersion(version);
                QVariant result;
                stream >> result;
                return result;
#else
                Q_ASSERT(!"stringToVariant: Cannot load custom types without QDataStream support");
#endif
#ifndef QT_NO_GEOM_VARIANT
            } else if (s.startsWith(QLatin1String("@Rect("))) {
                QStringList args = splitArgs(s, 5);
                if (args.size() == 4)
                    return QVariant(QRect(args[0].toInt(), args[1].toInt(), args[2].toInt(), args[3].toInt()));
            } else if (s.startsWith(QLatin1String("@Size("))) {
                QStringList args = splitArgs(s, 5);
                if (args.size() == 2)
                    return QVariant(QSize(args[0].toInt(), args[1].toInt()));
            } else if (s.startsWith(QLatin1String("@Point("))) {
                QStringList args = splitArgs(s, 6);
                if (args.size() == 2)
                    return QVariant(QPoint(args[0].toInt(), args[1].toInt()));
#endif
            } else if (s == QLatin1String("@Invalid()")) {
                return QVariant();
            }

        }
        if (s.startsWith(QLatin1String("@@")))
            return QVariant(s.mid(1));
    }

    return QVariant(s);
}

QStringList EasyIni::splitArgs(const QString &s, int idx)
{
    int l = s.length();
    Q_ASSERT(l > 0);
    Q_ASSERT(s.at(idx) == QLatin1Char('('));
    Q_ASSERT(s.at(l - 1) == QLatin1Char(')'));

    QStringList result;
    QString item;

    for (++idx; idx < l; ++idx) {
        QChar c = s.at(idx);
        if (c == QLatin1Char(')')) {
            Q_ASSERT(idx == l - 1);
            result.append(item);
        } else if (c == QLatin1Char(' ')) {
            result.append(item);
            item.clear();
        } else {
            item.append(c);
        }
    }

    return result;
}

