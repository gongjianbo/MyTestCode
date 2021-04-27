#include "settings.h"
#include <QtGui>

QMap<int, QString> comments;

//#define LZM_DEBUG

bool readIniComment(QString fileName)
{
    QFile file(fileName);
    if (!file.open(QIODevice::ReadOnly))
    {
        qDebug()<<"readIniComment open"<<fileName<<"failed!: error"<<file.error();
        return false;
    }

    QTextStream inStream(&file);
    inStream.setCodec("UTF-8");

    QString data;
    bool ok = true;

    int line=-1;
    while (!inStream.atEnd())
    {
        data = inStream.readLine();
        line++;
        QString trimmed = data.trimmed();
        if(trimmed.isEmpty())
        {
            continue;
        }
        else if(trimmed[0]== QChar('#') || trimmed[0]== QChar(';'))
        {
            comments[line] = data;
#ifdef LZM_DEBUG
            qDebug()<<"line"<<line<<"text"<<data;
#endif

        }
        else
        {
            continue;
        }
    }

    return ok;
}

bool WriteIniComment(QString fileName)
{
    QFile inFile(fileName);
    if (!inFile.open(QIODevice::ReadOnly))
    {
        qDebug()<<"WriteIniComment open"<<fileName<<"failed!";
        return false;
    }

    QString outFileName = fileName+".out";
    QFile outfile(outFileName);
    if (!outfile.open(QIODevice::WriteOnly))
    {
        qDebug()<<"open"<<outFileName<<"failed!";
        return false;
    }

    QTextStream inStream(&inFile);
    inStream.setCodec("UTF-8");

    QTextStream outStream(&outfile);
    outStream.setCodec("UTF-8");

    QString data;
    bool ok = true;

    int line=-1;

    while (!inStream.atEnd())
    {
        data = inStream.readLine();
        line++;
        QString trimmed = data.trimmed();
        //add comments
        while(comments.contains(line))
        {
            outStream <<comments[line]<<"\n";
#ifdef LZM_DEBUG
            qDebug()<<"line"<<line<<"text"<<comments[line];
#endif
            line++;
        }

        if(trimmed.isEmpty())
        {
            outStream <<data<<"\n";
            continue;
        }
        else
        {
            outStream <<data<<"\n";
        }
    }

    inFile.close();
    outfile.close();
//

    QFile::remove(fileName);
    qDebug()<<"remove"<<fileName;
    QFile::rename(outFileName, fileName);
    qDebug()<<"rename"<<outFileName<<"to"<<fileName;

    return ok;
}

QStringList splitArgs(const QString &s, int idx)
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

QVariant stringToVariant(const QString &s)
{
    if (s.startsWith(QLatin1Char('@'))) {
        if (s.endsWith(QLatin1Char(')'))) {
            if (s.startsWith(QLatin1String("@ByteArray("))) {
                return QVariant(s.toLatin1().mid(11, s.size() - 12));
            } else if (s.startsWith(QLatin1String("@Variant("))) {
#ifndef QT_NO_DATASTREAM
                QByteArray a(s.toLatin1().mid(9));
                QDataStream stream(&a, QIODevice::ReadOnly);
                stream.setVersion(QDataStream::Qt_4_0);
                QVariant result;
                stream >> result;
                return result;
#else
                Q_ASSERT(!"QSettings: Cannot load custom types without QDataStream support");
#endif
#ifndef QT_NO_GEOM_VARIANT
            } else if (s.startsWith(QLatin1String("@Rect("))) {
                QStringList args = ::splitArgs(s, 5);
                if (args.size() == 4)
                    return QVariant(QRect(args[0].toInt(), args[1].toInt(), args[2].toInt(), args[3].toInt()));
            } else if (s.startsWith(QLatin1String("@Size("))) {
                QStringList args = ::splitArgs(s, 5);
                if (args.size() == 2)
                    return QVariant(QSize(args[0].toInt(), args[1].toInt()));
            } else if (s.startsWith(QLatin1String("@Point("))) {
                QStringList args = ::splitArgs(s, 6);
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

bool myReadFunc(QIODevice &device, QSettings::SettingsMap &settingsMap)
{
        QString currentSection;
        QTextStream stream(&device);
        stream.setCodec("UTF-8");
        QString data;
        bool ok = true;
        while (!stream.atEnd())
        {
            data = stream.readLine();
            if (data.trimmed().isEmpty())
            {
                continue;
            }
            if (data[0] == QChar('['))
            {
                QString iniSection;
                int inx = data.indexOf(QChar(']'));
                if (inx == -1){
                    ok = false;
                    iniSection = data.mid(1);
                }
                else
                {
                    iniSection = data.mid(1, inx - 1);
                }
                iniSection = iniSection.trimmed();
                if (iniSection.compare(QString("general"), Qt::CaseInsensitive) == 0)
                {
                    currentSection.clear();
                }
                else
                {
                    if (iniSection.compare(QString("%general"), Qt::CaseInsensitive) == 0)
                    {
                        currentSection = QString("general");
                    }
                    else
                    {
                        currentSection = iniSection;
                    }
                    currentSection += QChar('/');

                }
            }
            else
            {
                bool inQuotes = false;
                int equalsPos = -1;
                QList<int> commaPos;
                int i = 0;
                while (i < data.size())
                {
                    QChar ch = data.at(i);
                    if (ch == QChar('='))
                    {
                        if (!inQuotes && equalsPos == -1)
                        {
                            equalsPos = i;
                        }
                    }
                    else if (ch == QChar('"'))
                    {
                        inQuotes = !inQuotes;
                    }
                    else if (ch == QChar(','))
                    {
                        if (!inQuotes && equalsPos != -1)
                        {
                            commaPos.append(i);
                        }
                    }
                    else if (ch == QChar(';') || ch == QChar('#'))
                    {
                        if (!inQuotes)
                        {
                            data.resize(i);
                            break;
                        }
                    }
                    else if (ch == QChar('\\'))
                    {
                        if (++i < data.size())
                        {
                        }
                        else
                        {
                            ok = false;
                            break;
                        }
                    }
                    i++;
                }
                if (equalsPos == -1)
                {
                    break;
                }
                else
                {
                    QString key = data.mid(0, equalsPos).trimmed();
                    if (key.isEmpty())
                    {
                        break;
                    }
                    else
                    {
                        key = currentSection + key;
                    }
                    if (commaPos.isEmpty())
                    {
                        //value
                        QString v = data.mid(equalsPos+1).trimmed();
                        if (v.startsWith("\"") && v.endsWith("\"") && v.length()>1)
                        {
                            v = v.mid(1, v.length()-2);
                        }
                        settingsMap[key] = stringToVariant(v);
                    }
                    else
                    { //value list
                        commaPos.prepend(equalsPos);
                        commaPos.append(-1);
                        QVariantList vals;
                        for (int i=1; i<commaPos.size(); ++i)
                        {
                            QString d = data.mid(commaPos.at(i-1)+1, commaPos.at(i)-commaPos.at(i-1)-1);
                            QString v = d.trimmed();
                            if (v.startsWith("\"") && v.endsWith("\"") && v.length()>1)
                            {
                                v = v.mid(1, v.length()-2);
                            }
                            vals.append(stringToVariant(v) /*stringToVariant(unescapedString(v))*/);
                        }
                        settingsMap[key] = vals;
                    }
                }
            }
        }

        return ok;
}

QString variantToString(const QVariant &v)
{
     QString result;

     switch (v.type()) {
         case QVariant::Invalid:
             result = QLatin1String("@Invalid()");
             break;

         case QVariant::ByteArray: {
             QByteArray a = v.toByteArray();
             result = QLatin1String("@ByteArray(");
             result += QString::fromLatin1(a.constData(), a.size());
             result += QLatin1Char(')');
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
             if (result.startsWith(QLatin1Char('@')))
                 result.prepend(QLatin1Char('@'));
             break;
         }
#ifndef QT_NO_GEOM_VARIANT
         case QVariant::Rect: {
             QRect r = qvariant_cast<QRect>(v);
             result += QLatin1String("@Rect(");
             result += QString::number(r.x());
             result += QLatin1Char(' ');
             result += QString::number(r.y());
             result += QLatin1Char(' ');
             result += QString::number(r.width());
             result += QLatin1Char(' ');
             result += QString::number(r.height());
             result += QLatin1Char(')');
             break;
         }
         case QVariant::Size: {
             QSize s = qvariant_cast<QSize>(v);
             result += QLatin1String("@Size(");
             result += QString::number(s.width());
             result += QLatin1Char(' ');
             result += QString::number(s.height());
             result += QLatin1Char(')');
             break;
         }
         case QVariant::Point: {
             QPoint p = qvariant_cast<QPoint>(v);
             result += QLatin1String("@Point(");
             result += QString::number(p.x());
             result += QLatin1Char(' ');
             result += QString::number(p.y());
             result += QLatin1Char(')');
             break;
         }
#endif // !QT_NO_GEOM_VARIANT

         default: {
#ifndef QT_NO_DATASTREAM
             QByteArray a;
             {
                 QDataStream s(&a, QIODevice::WriteOnly);
                 s.setVersion(QDataStream::Qt_4_0);
                 s << v;
             }

             result = QLatin1String("@Variant(");
             result += QString::fromLatin1(a.constData(), a.size());
             result += QLatin1Char(')');
#else
             Q_ASSERT(!"QSettings: Cannot save custom types without QDataStream support");
#endif
             break;
         }
     }

     return result;
}

bool myWriteFunc(QIODevice &device, const QSettings::SettingsMap &settingsMap)
{
#ifdef Q_OS_WIN
    const char * const eol = "\r\n";
#else
    const char *eol = "\n";
#endif
    bool writeError = false;
    QString lastSection;
    QMapIterator<QString,QVariant> it(settingsMap);
    while(it.hasNext() && !writeError)
    {
        it.next();
        QString key = it.key();
        QString section;
        qDebug()<<"key: "<<key;
        int idx = key.lastIndexOf(QChar('/'));
        if (idx == -1)
        {
            section = QString("[General]");
        }
        else
        {
            section = key.left(idx);
            key = key.mid(idx+1);
            if (section.compare(QString("General"), Qt::CaseInsensitive) == 0)
            {
                section = QString("[%General]");
            }
            else
            {
                section.prepend(QChar('['));
                section.append(QChar(']'));
            }
        }
        if (section.compare(lastSection, Qt::CaseInsensitive))
        {
            if (!lastSection.isEmpty())
            {
                device.write(eol);
            }
            lastSection = section;
            if (device.write(section.toUtf8() + eol) == -1)
            {
                writeError = true;
            }
        }
        QByteArray block = key.toUtf8();
        block += " = ";
        if (it.value().type() == QVariant::StringList)
        {
//            foreach (QString s, it.value().toStringList())
//            {
//                block += /*escapedString(s)*/ s;
//                block += ", ";
//            }
//            if (block.endsWith(", "))
//            {
//                block.chop(2);
//            }
            qDebug()<< "no support QVariant::StringList";
        }
        else if (it.value().type() == QVariant::List)
        {
//            foreach (QVariant v, it.value().toList())
//            {
//                block += /*escapedString(variantToString(v))*/ v.toString();
//                block += ", ";
//            }
//            if (block.endsWith(", "))
//            {
//                block.chop(2);
//            }
            qDebug()<< "no support QVariant::List";
        }
        else
        {
//            block += /*escapedString*/(variantToString(it.value()));
//            block += it.value().toString();
            block += variantToString(it.value());
        }
        block += eol;
        if (device.write(block) == -1)
        {
            writeError = true;
        }
    }
    return writeError;

}

Settings::Settings(const QString & fileName)
{
    readIniComment(fileName);
    const QSettings::Format MyIniFormat = QSettings::registerFormat("ini", myReadFunc, myWriteFunc);
    d= new QSettings(fileName, MyIniFormat);
    d->setIniCodec("UTF-8");
}

Settings::~Settings()
{
    if(d)
    {
        QString fileName = d->fileName();

        delete d;
        WriteIniComment(fileName);
    }

}

void Settings::setValue(const QString &key, const QVariant &value)
{
    d->setValue(key, value);
}

QVariant Settings::value(const QString &key, const QVariant &defaultValue) const
{
    return d->value(key, defaultValue);
}
