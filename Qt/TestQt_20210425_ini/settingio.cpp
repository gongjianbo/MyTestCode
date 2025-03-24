// Copyright (C) 2019 ~ 2020 Uniontech Software Technology Co.,Ltd.
// SPDX-FileCopyrightText: 2023 UnionTech Software Technology Co., Ltd.
//
// SPDX-License-Identifier: GPL-3.0-or-later

#include "settingio.h"

#include <QTextStream>
#include <qdatastream.h>
#include <QDebug>
#include <QVariant>
#include <QTextCodec>

#define SLASH_REPLACE_CHAR QChar(0x01)
bool SettingIO::rewrite = false;
SettingIO::SettingIO(QObject *parent) : QObject(parent)
{

}

bool SettingIO::readIniFunc(QIODevice &device, QSettings::SettingsMap &settingsMap)
{
    QString currentSection;
    QTextStream stream(&device);
    stream.setCodec("UTF-8");
    QString data;
    rewrite = false;
    while (!stream.atEnd()) {
        data = stream.readLine();
        if (data.trimmed().isEmpty())
            continue;

        if (QChar('[') == data[0]) {
            QString iniSection;
            int inx = data.lastIndexOf(QChar(']'));
            if (-1 == inx)
                iniSection = data.mid(1);
            else
                iniSection = data.mid(1, inx - 1);

            iniSection = iniSection.trimmed();
            if (0 == iniSection.compare(QString("general"), Qt::CaseInsensitive)) {
                currentSection.clear();
            } else {
                if (0 == iniSection.compare(QString("%general"), Qt::CaseInsensitive)) {
                    currentSection = QString("general");
                } else {
                    iniSection = canTransfer(iniSection);
                    currentSection = iniSection;
                }
                currentSection.replace('/', SLASH_REPLACE_CHAR);
                currentSection += QChar('/');
            }
        } else {
            bool inQuotes = false;
            int equalsPos = -1;
            QList<int> commaPos;
            int charPos = 0;
            while (charPos < data.size()) {
                QChar ch = data.at(charPos);
                if (QChar('=') == ch) {
                    if (!inQuotes && equalsPos == -1)
                        equalsPos = charPos;
                } else if (QChar('"') == ch) {
                    inQuotes = !inQuotes;
                }
                charPos++;
            }
            if (-1 == equalsPos) {
                break;
            } else {
                QString key = data.mid(0, equalsPos).trimmed();
                if (key.isEmpty())
                    break;
                else
                    key = currentSection + key;

                if (commaPos.isEmpty()) { //value
                    QString v = data.mid(equalsPos + 1).trimmed();
                    if (v.startsWith("\"") && v.endsWith("\"") && v.length() > 1)
                        v = v.mid(1, v.length() - 2);

                    QString str = unescapedString(v);
                    //如果是3字节的Latin1中文，转换中文编码
                    if (v.count("\\x") > 0 && 0 == v.count("\\x") % 3) {
                        int first = v.indexOf("\\x");
                        int next = v.indexOf("\\x", first + 1);
                        int len = next - first;
                        if (4 == len)
                            str = QString::fromLocal8Bit(str.toLatin1());
                    }

                    settingsMap[key] = stringToVariant(str);
                } else { //value list
                    commaPos.prepend(equalsPos);
                    commaPos.append(-1);
                    QVariantList vals;
                    for (int pos = 1; pos < commaPos.size(); ++pos) {
                        QString d = data.mid(commaPos.at(pos - 1) + 1, commaPos.at(pos) - commaPos.at(pos - 1) - 1);
                        QString v = d.trimmed();
                        if (v.startsWith("\"") && v.endsWith("\"") && v.length() > 1)
                            v = v.mid(1, v.length() - 2);

                        vals.append(stringToVariant(unescapedString(v)));
                    }
                    settingsMap[key] = vals;
                }
            }
        }
    }
    return true;
}

bool SettingIO::writeIniFunc(QIODevice &device, const QSettings::SettingsMap &settingsMap)
{
#ifdef Q_OS_WIN
    const char *const eol = "\r\n";
#else
    const char *eol = "\n";
#endif
    bool writeError = false;

    QString lastSection;
    QMapIterator<QString, QVariant> it(settingsMap);
    while (it.hasNext() && !writeError) {
        it.next();
        QString key = it.key();
        QString section;
        int idx = key.lastIndexOf(QChar('/'));
        if (-1 == idx) {
            section = QString("[General]");
        } else {
            section = key.left(idx);
            key = key.mid(idx + 1);
            if (0 == section.compare(QString("General"), Qt::CaseInsensitive)) {
                section = QString("[%General]");
            } else {
                section.prepend(QChar('['));
                section.append(QChar(']'));
            }

            section.replace(SLASH_REPLACE_CHAR, '/');
        }
        if (section.compare(lastSection, Qt::CaseInsensitive)) {
            if (!lastSection.isEmpty())
                device.write(eol);

            lastSection = section;
            if (-1 == device.write(section.toUtf8() + eol))
                writeError = true;
        }
        const QByteArray dot = ", ";
        QByteArray block = key.toUtf8();
        block += " = ";
        if (QVariant::StringList == it.value().type()) {
            foreach (QString s, it.value().toStringList()) {
                block += escapedString(s);
                block += dot;
            }
            if (block.endsWith(dot))
                block.chop(2);

        } else if (QVariant::List == it.value().type()) {
            foreach (QVariant v, it.value().toList()) {
                block += escapedString(variantToString(v));
                block += dot;
            }
            if (block.endsWith(dot))
                block.chop(2);

        } else {
            block += escapedString(variantToString(it.value()));
        }
        block += eol;
        if (device.write(block) == -1)
            writeError = true;
    }
    return true;
}

QString SettingIO::variantToString(const QVariant &v)
{
    QString result;
    switch (v.type()) {
    case QVariant::String:
    case QVariant::LongLong:
    case QVariant::ULongLong:
    case QVariant::Int:
    case QVariant::UInt:
    case QVariant::Bool:
    case QVariant::Double:
    case QVariant::KeySequence: {
        result = v.toString();
        if (result.startsWith(QChar('@')))
            result.prepend(QChar('@'));
        break;
    }
    default: {
        QByteArray a;
        {
            QDataStream s(&a, QIODevice::WriteOnly);
            s.setVersion(QDataStream::Qt_4_0);
            s << v;
        }

        result = QString("@Variant(");
        result += QString::fromLatin1(a.constData(), a.size());
        result += QChar(')');
        break;
    }
    }

    return result;
}

QVariant SettingIO::stringToVariant(const QString &s)
{
    if (s.startsWith(QChar('@'))) {
        if (s.endsWith(QChar(')'))) {
            if (s.startsWith(QString("@Variant("))) {
                QByteArray a(s.toUtf8().mid(9));
                QDataStream stream(&a, QIODevice::ReadOnly);
                stream.setVersion(QDataStream::Qt_4_0);
                QVariant result;
                stream >> result;
                return result;
            }
        }
        if (s.startsWith(QString("@@")))
            return QVariant(s.mid(1));
    }
    return QVariant(s);
}

QByteArray SettingIO::escapedString(const QString &src)
{
    bool needsQuotes = false;
    bool escapeNextIfDigit = false;
    QByteArray result;
    result.reserve(src.size() * 3 / 2);
    for (int i = 0; i < src.size(); ++i) {
        uint ch = src.at(i).unicode();
        if (';' == ch || ',' == ch || '=' == ch || '#' == ch)
            needsQuotes = true;

        if (escapeNextIfDigit && ((ch >= '0' && ch <= '9')
                                  || (ch >= 'a' && ch <= 'f') || (ch >= 'A' && ch <= 'F'))) {
            result += "\\x";
            result += QByteArray::number(ch, 16);
            continue;
        }

        escapeNextIfDigit = false;

        switch (ch) {
        case '\0':
            result += "\\0";
            escapeNextIfDigit = true;
            break;
        case '\n':
            result += "\\n";
            break;
        case '\r':
            result += "\\r";
            break;
        case '\t':
            result += "\\t";
            break;
        case '"':
        case '\\':
            result += '\\';
            result += (char)ch;
            break;
        default:
            if (ch <= 0x1F) {
                result += "\\x";
                result += QByteArray::number(ch, 16);
                escapeNextIfDigit = true;
            } else {
                result += QString(src[i]).toUtf8();
            }
            break;
        }
    }
    if (result.size() > 0 && (' ' == result.at(0) || ' ' == result.at(result.size() - 1)))
        needsQuotes = true;

    if (needsQuotes) {
        result.prepend('"');
        result.append('"');
    }
    return result;
}


const char hexDigits[] = "0123456789ABCDEF";

QString SettingIO::unescapedString(const QString &src)
{
    static const char escapeCodes[][2] = {
        { 'a', '\a' },
        { 'b', '\b' },
        { 'f', '\f' },
        { 'n', '\n' },
        { 'r', '\r' },
        { 't', '\t' },
        { 'v', '\v' },
        { '"',  '"' },
        { 's',   ' '},
        { '?',  '?' },
        { '\'', '\'' },
        { '\\', '\\' }

    };
    static const int numEscapeCodes = sizeof(escapeCodes) / sizeof(escapeCodes[0]);

    QString stringResult;
    int escapeVal = 0;
    QChar ch;
    int i = 0;
normal:
    while (i < src.size()) {
        ch = src.at(i);
        if (QChar('\\') == ch) {
            ++i;
            if (i >= src.size())
                break;

            ch = src.at(i++);
            for (int j = 0; j < numEscapeCodes; ++j) {
                if (ch == escapeCodes[j][0]) {
                    stringResult += QChar(escapeCodes[j][1]);
                    goto normal;
                }
            }
            if ('x' == ch) {
                escapeVal = 0;
                if (i >= src.size())
                    break;
                ch = src.at(i);
                if ((ch >= '0' && ch <= '9') || (ch >= 'A' && ch <= 'F') || (ch >= 'a' && ch <= 'f'))
                    goto hexEscape;
            } else if (ch >= '0' && ch <= '7') {
                escapeVal = ch.unicode() - '0';
                goto octEscape;
            } else {
                //skip
            }
        } else {
            stringResult += ch;
        }
        i++;
    }
    goto end;

hexEscape:
    if (i >= src.size()) {
        stringResult += QChar(escapeVal);
        goto end;
    }

    ch = src.at(i);
    if (ch >= 'a')
        ch = ch.unicode() - ('a' - 'A');
    if ((ch >= '0' && ch <= '9') || (ch >= 'A' && ch <= 'F')) {
        escapeVal <<= 4;
        escapeVal += strchr(hexDigits, ch.toLatin1()) - hexDigits;
        ++i;
        goto hexEscape;
    } else {
        stringResult += QChar(escapeVal);
        goto normal;
    }

octEscape:
    if (i >= src.size()) {
        stringResult += QChar(escapeVal);
        goto end;
    }

    ch = src.at(i);
    if (ch >= '0' && ch <= '7') {
        escapeVal <<= 3;
        escapeVal += ch.toLatin1() - '0';
        ++i;
        goto octEscape;
    } else {
        stringResult += QChar(escapeVal);
        goto normal;
    }

end:
    return stringResult;
}

QString SettingIO::canTransfer(const QString &str)
{
    QString res;
    //如果有%U，是Uincode字符串
    if (str.contains("%U") || str.contains("%u")) {
        rewrite = true;
        //uincode
        iniUnescapedKey(str.toLocal8Bit(), 0, str.size(), res);
    }
    //如果是%是Latin1格式的字符串
    else if (str.contains("%")) {
        rewrite = true;
        //utf-8转换为uincode过了
        iniUnescapedKey(str.toLocal8Bit(), 0, str.size(), res);
        res = QString::fromLocal8Bit(res.toLatin1());
    } else {
        res = str;
    }

    return res;
}

bool SettingIO::iniUnescapedKey(const QByteArray &key, int from, int to, QString &result)
{
    bool lowercaseOnly = true;
    int i = from;
    result.reserve(result.length() + (to - from));
    while (i < to && i < key.count()) {
        int ch = (uchar)key.at(i);

        if ('\\' == ch) {
            result += QLatin1Char('/');
            ++i;
            continue;
        }

        if (ch != '%' || i == to - 1) {
            if (uint(ch - 'A') <= 'Z' - 'A') // only for ASCII
                lowercaseOnly = false;
            result += QLatin1Char(ch);
            ++i;
            continue;
        }

        int numDigits = 2;
        int firstDigitPos = i + 1;

        ch = key.at(i + 1);
        if ('U' == ch) {
            ++firstDigitPos;
            numDigits = 4;
        }

        if (firstDigitPos + numDigits > to) {
            result += QLatin1Char('%');
            // ### missing U
            ++i;
            continue;
        }

        bool ok;
        ch = key.mid(firstDigitPos, numDigits).toInt(&ok, 16);
        if (!ok) {
            result += QLatin1Char('%');
            // ### missing U
            ++i;
            continue;
        }

        QChar qch(ch);
        if (qch.isUpper())
            lowercaseOnly = false;
        result += qch;
        i = firstDigitPos + numDigits;
    }
    return lowercaseOnly;
}

//自定义规则
QSettings::Format USettings::g_customFormat = QSettings::registerFormat("conf", SettingIO::readIniFunc, SettingIO::writeIniFunc);

//构造函数
USettings::USettings(const QString &fileName, QObject *parent)
    : QSettings(fileName, g_customFormat, parent)
{
    QSettings::setIniCodec(QTextCodec::codecForName("UTF-8"));
}

//析构函数
USettings::~USettings() {}

void USettings::beginGroup(const QString &prefix)
{
    QString tempPrefix = prefix;
    tempPrefix.replace('/', SLASH_REPLACE_CHAR);
    QSettings::beginGroup(tempPrefix);
}

void USettings::endGroup()
{
    QSettings::endGroup();
}

void USettings::setValue(const QString &key, const QVariant &value)
{
    QString tempKey = key;
    tempKey.replace('/', SLASH_REPLACE_CHAR);
    QSettings::setValue(tempKey, value);
}

QVariant USettings::value(const QString &key, const QVariant &defaultValue) const
{
    QString tempKey = key;
    tempKey.replace('/', SLASH_REPLACE_CHAR);
    QVariant value = QSettings::value(tempKey, defaultValue);
    return value.toString().replace(SLASH_REPLACE_CHAR, '/');
}

void USettings::remove(const QString &key)
{
    QString tempKey = key;
    tempKey.replace('/', SLASH_REPLACE_CHAR);
    QSettings::remove(tempKey);
}

bool USettings::contains(const QString &key) const
{
    QString tempKey = key;
    tempKey.replace('/', SLASH_REPLACE_CHAR);
    return QSettings::contains(tempKey);
}

QStringList USettings::childGroups()
{
    QStringList childGroups = QSettings::childGroups();
    for (QString &child : childGroups) {
        child.replace(SLASH_REPLACE_CHAR, '/');
    }
    return childGroups;
}
