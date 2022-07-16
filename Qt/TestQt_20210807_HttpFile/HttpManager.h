#pragma once
#include <QObject>
#include <QNetworkAccessManager>
#include <QNetworkRequest>
#include <QNetworkReply>
#include <QHttpPart>
#include <QHttpMultiPart>
#include <QUrl>
#include <QUrlQuery>
#include <QJsonDocument>
#include <QJsonParseError>
#include <QJsonValue>
#include <QJsonObject>
#include <QJsonArray>
#include <QFile>

class HttpManager : public QObject
{
    Q_OBJECT
public:
    explicit HttpManager(QObject *parent = nullptr);

    Q_INVOKABLE void upload(const QString &url, const QString &filepath);
    Q_INVOKABLE void download(const QString &url, const QString &fileid, const QString &filepath);

private:
    QNetworkAccessManager manager;
};

