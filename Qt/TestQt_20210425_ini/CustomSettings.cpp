#include "CustomSettings.h"
#include <QTextCodec>
#include <QTextStream>
#include <QDataStream>
#include <QFileInfo>
#include <QFile>
#include <QDir>
#include <QRect>
#include <QSize>
#include <QDebug>

QTextCodec *CustomSettingsIO::iniCodec = QTextCodec::codecForName("UTF-16LE");

bool CustomSettingsIO::readIniFunc(QIODevice &device, QSettings::SettingsMap &settingsMap)
{
    // QTextStream会检测utf16/32的bom，没bom就用设置的编码，否则默认system
    QTextStream stream(&device);
    stream.setCodec(parseCodec(device));

    QString currentSection;
    QString line;
    while (!stream.atEnd()) {
        // 去掉首位空格
        line = stream.readLine().trimmed();
        // 跳过注释或者无效行，[a]/a=b一行至少3个字符
        if (line.count() < 3 ||
            line[0] == ';' ||
            line[0] == '#' ||
            line[0] == '=')
            continue;

        if (QChar('[') == line[0]) {
            // 取section段名
            QString section;
            int idx = line.lastIndexOf(QChar(']'));
            if (-1 == idx)
                section = line.mid(1);
            else
                section = line.mid(1, idx - 1);
            section = section.trimmed();
            // QSettings默认会把未分组的放到general分组，用户general分组放到%general分组
            // 考虑到general比较常用，改为放custom_settings分组，且不转义用户已有的分组
            if (0 == section.compare(QString("custom_settings"), Qt::CaseInsensitive)) {
                currentSection.clear();
            } else {
                currentSection = section;
                // 加的/分隔符是放到QSettings::SettingsMap时和ini-key拼接成map-key用的
                currentSection += QChar('/');
            }
        } else {
            // 取key-value键值对
            int idx = line.indexOf('=');
            if (-1 == idx)
                continue;
            QString key = line.mid(0, idx).trimmed();
            QString value = line.mid(idx + 1, line.length() - idx - 1).trimmed();
            key = currentSection + key;
            settingsMap[key] = stringToVariant(value);
            // qDebug() << "insert" << key << value;
        }
    }
    return true;
}

bool CustomSettingsIO::writeIniFunc(QIODevice &device, const QSettings::SettingsMap &settingsMap)
{
    // 因为device不是Text模式打开的，所以Qt::endl不会根据平台设置
#ifdef Q_OS_WIN
    const char *const eol = "\r\n";
#else
    const char *eol = "\n";
#endif
    QTextStream stream(&device);
    // 设置编码
    stream.setCodec(iniCodec);
    // 写入编码BOM
    stream.setGenerateByteOrderMark(true);

    bool writeError = false;
    QString lastSection;
    QMapIterator<QString, QVariant> it(settingsMap);
    while (it.hasNext() && !writeError) {
        it.next();
        QString key = it.key();
        QString section;
        int idx = key.indexOf(QChar('/'));
        if (-1 == idx) {
            // QSettings默认会把未分组的放到general分组，用户general分组放到%general分组
            // 考虑到general比较常用，改为放custom_settings分组，且不转义用户已有的分组
            section = QString("custom_settings");
        } else {
            section = key.left(idx);
            key = key.mid(idx + 1);
            section.prepend(QChar('['));
            section.append(QChar(']'));
        }
        if (section.compare(lastSection, Qt::CaseInsensitive)) {
            if (!lastSection.isEmpty())
                stream << eol;
            lastSection = section;
            stream << section << eol;
            if (stream.status() != QTextStream::Ok)
                writeError = true;
        }
        stream << key << "=" << variantToString(it.value()) << eol;
        if (stream.status() != QTextStream::Ok)
            writeError = true;
    }
    return true;
}

QTextCodec *CustomSettingsIO::parseCodec(QIODevice &device)
{
    // 不识别的都当作UTF-8
    QByteArray codec_name = "UTF-8";
    do {
        QByteArray buf = device.readLine(4);
        device.seek(0);
        quint8 b1, b2, b3;
        if (buf.size() < 2) {
            break;
        } else {
            b1 = buf.at(0);
            b2 = buf.at(1);
        }
        if (b1 == 0xFF && b2 == 0xFE) {
            codec_name = "UTF-16LE";
            break;
        } else if (b1 == 0xFE && b2 == 0xFF) {
            codec_name = "UTF-16BE";
            break;
        }
        if (buf.size() < 3) {
            break;
        } else {
            b3 = buf.at(2);
        }
        if (b1 == 0xEF && b2 == 0xBB && b3 == 0xBF) {
            // utf-8带bom
            codec_name = "UTF-8";
        } else {
            // 对于不带bom的文件需要全文检测
            buf = device.readAll();
            device.seek(0);
            // 尝试用utf8转换，如果无效字符数大于0，则非utf8编码
            QTextCodec::ConverterState state;
            QTextCodec *codec = QTextCodec::codecForName("UTF-8");
            codec->toUnicode(buf.constData(), buf.size(), &state);
            if (state.invalidChars > 0) {
                codec_name = "System";
            }
        }
    } while(false);
    // qDebug() << "ini codec" << codec_name;
    return QTextCodec::codecForName(codec_name);
}

QString CustomSettingsIO::variantToString(const QVariant &v)
{
    QString result;

    switch (v.userType()) {
    case QMetaType::UnknownType:
        result = QLatin1String("@Invalid()");
        break;

    case QMetaType::QStringList:
    case QMetaType::QVariantList: {
        // VariantList暂时用不到就不做额外处理了，统一作为StringList
        QStringList v_list = v.toStringList();
        result = v_list.join(", ");
        break;
    }

    case QMetaType::QByteArray: {
        QByteArray a = v.toByteArray();
        result = QLatin1String("@ByteArray(")
                 + QLatin1String(a.constData(), a.size())
                 + QLatin1Char(')');
        break;
    }

    case QMetaType::QString:
    case QMetaType::LongLong:
    case QMetaType::ULongLong:
    case QMetaType::Int:
    case QMetaType::UInt:
    case QMetaType::Bool:
    case QMetaType::Double:
    case QMetaType::QKeySequence: {
        result = v.toString();
        if (result.contains(QChar::Null))
            result = QLatin1String("@String(") + result + QLatin1Char(')');
        else if (result.startsWith(QLatin1Char('@')))
            result.prepend(QLatin1Char('@'));
        break;
    }
#ifndef QT_NO_GEOM_VARIANT
    case QMetaType::QRect: {
        QRect r = qvariant_cast<QRect>(v);
        result = QString::asprintf("@Rect(%d %d %d %d)", r.x(), r.y(), r.width(), r.height());
        break;
    }
    case QMetaType::QSize: {
        QSize s = qvariant_cast<QSize>(v);
        result = QString::asprintf("@Size(%d %d)", s.width(), s.height());
        break;
    }
    case QMetaType::QPoint: {
        QPoint p = qvariant_cast<QPoint>(v);
        result = QString::asprintf("@Point(%d %d)", p.x(), p.y());
        break;
    }
#endif // !QT_NO_GEOM_VARIANT

    default: {
#ifndef QT_NO_DATASTREAM
        QDataStream::Version version;
        const char *typeSpec;
        if (v.userType() == QMetaType::QDateTime) {
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
        Q_ASSERT(!"CustomSettings: Cannot save custom types without QDataStream support");
#endif
        break;
    }
    }

    return result;
}

QVariant CustomSettingsIO::stringToVariant(const QString &s)
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
                Q_ASSERT(!"CustomSettings: Cannot load custom types without QDataStream support");
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

QStringList CustomSettingsIO::splitArgs(const QString &s, int idx)
{
    QStringList result;
    int l = s.length();
    if (!(l > 0) || !(s.at(idx) == QLatin1Char('(')) || !(s.at(l - 1) == QLatin1Char(')')))
        return result;

    QString item;
    for (++idx; idx < l; ++idx) {
        QChar c = s.at(idx);
        if (c == QLatin1Char(')')) {
            if (!(idx == l - 1))
                return result;
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

// 注册自定义规则
QSettings::Format CustomSettings::customIniFormat = QSettings::registerFormat(
    "custom_settings",
    CustomSettingsIO::readIniFunc,
    CustomSettingsIO::writeIniFunc);

CustomSettings::CustomSettings(const QString &fileName, QObject *parent)
    : QSettings(fileName, customIniFormat, parent)
{

}

CustomSettings::~CustomSettings()
{

}
