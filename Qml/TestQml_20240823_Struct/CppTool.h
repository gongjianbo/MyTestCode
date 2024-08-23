#pragma once
#include <QObject>
#include <QSharedData>
#include <QJsonValue>
#include <QJsonObject>
#include <QJsonArray>
#include <QImage>
#include <QPixmap>
#include <QPicture>
#include <QBitmap>
#include <QVector>
#include <QList>
#include <qqml.h>

struct CppData
{
    Q_GADGET
    Q_PROPERTY(qint64 id MEMBER id)
    Q_PROPERTY(QString name MEMBER name)
public:
    Q_INVOKABLE CppData();
    CppData(const CppData &other);
    ~CppData();
    CppData &operator =(const CppData &other);

    Q_INVOKABLE QString toString();

    qint64 id{0};
    QString name;
    QByteArray data;
};
Q_DECLARE_METATYPE(CppData)

class CppTool : public QObject
{
    Q_OBJECT
    Q_DISABLE_COPY_MOVE(CppTool)
private:
    explicit CppTool(QObject *parent = nullptr);
public:
    ~CppTool();
    static CppTool *getInstance();

    Q_INVOKABLE CppData createData();
    Q_INVOKABLE void setData(const CppData &data);

    Q_INVOKABLE QVariant createMeta();
    Q_INVOKABLE void setMeta(const QVariant &var);
};
