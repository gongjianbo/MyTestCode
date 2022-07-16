#include "HttpManager.h"
#include <QEventLoop>
#include <QFileInfo>
#include <QDebug>

HttpManager::HttpManager(QObject *parent) : QObject(parent)
{
    //QNetworkReply自动释放
    manager.setAutoDeleteReplies(true);
}

void HttpManager::upload(const QString &url, const QString &filepath)
{
    qDebug()<<"[upload file]"<<url<<QFileInfo(filepath).fileName();
    QFile *file=new QFile(filepath);
    if(!file->open(QIODevice::ReadOnly)){
        file->deleteLater();
        qDebug()<<"open file error";
        return;
    }

    QNetworkRequest request;
    request.setUrl(QUrl(url));
    //request.setRawHeader("Content-Type","multipart/form-data");

    //QHttpMultiPart需要在请求完成后释放
    QHttpMultiPart *multi_part = new QHttpMultiPart(QHttpMultiPart::FormDataType);
    QHttpPart file_part;
    file_part.setHeader(QNetworkRequest::ContentDispositionHeader,
                        QVariant(QString("form-data; name=\"myfile\"; filename=\"%1\";")
                                 .arg(QFileInfo(filepath).fileName())));
    //part.header加上这句flask.request.files才能拿到form-data的信息
    //注意不是request的header
    file_part.setRawHeader("Content-Type", "multipart/form-data");
    file_part.setBodyDevice(file);
    file->setParent(multi_part);
    multi_part->append(file_part);

    QNetworkReply *reply = manager.post(request,multi_part);
    multi_part->setParent(reply); //在删除reply时一并释放

    //因为是测试所以同步等待
    QEventLoop eventLoop;
    //上传进度
    connect(reply, &QNetworkReply::uploadProgress,
            this, [this](qint64 bytesSent, qint64 bytesTotal){
        qDebug()<<"[upload file] bytesSend"<<bytesSent<<"bytesTotal"<<bytesTotal;
    });
    //结束退出事件循环
    connect(reply, &QNetworkReply::finished, &eventLoop, &QEventLoop::quit);
    eventLoop.exec();

    int status_code = reply->attribute(QNetworkRequest::HttpStatusCodeAttribute).toInt();
    qDebug()<<"reply"<<status_code<<QString(reply->readAll());
    qDebug()<<"[upload file] finished";
}

void HttpManager::download(const QString &url, const QString &fileid, const QString &filepath)
{
    qDebug()<<"[download file]"<<url<<fileid;

    QNetworkRequest request;
    request.setUrl(QUrl(url+QString("/%1").arg(fileid)));
    QNetworkReply *reply = manager.get(request);

    //先删除已有的
    QFile file(filepath);
    file.remove();

    //因为是测试所以同步等待
    QEventLoop eventLoop;
    //数据可读
    connect(reply, &QNetworkReply::readyRead, this, [this,reply,&file](){
           if(!reply->isOpen()){
               if(!reply->open(QIODevice::ReadOnly)){
                   qDebug()<<"[download file] reply open failed";
                   return;
               }
           }
           if(!file.isOpen()){
               if(!file.open(QIODevice::WriteOnly | QIODevice::Append)){
                   qDebug()<<"[download file] file open failed";
                   return;
               }
           }
           file.write(reply->readAll());
    });
    //下载进度
    connect(reply, &QNetworkReply::downloadProgress,
            this, [this](qint64 bytesReceived, qint64 bytesTotal){
        qDebug()<<"[download file] bytesReceived"<<bytesReceived<<"bytesTotal"<<bytesTotal;
    });
    //结束退出事件循环
    connect(reply, &QNetworkReply::finished, &eventLoop, &QEventLoop::quit);
    eventLoop.exec();
    file.close(); //关闭文件

    int status_code = reply->attribute(QNetworkRequest::HttpStatusCodeAttribute).toInt();
    //如果是无效的响应吧数据清除
    if(status_code != 200)
        file.remove();
    qDebug()<<"reply"<<status_code<<QString(reply->readAll());
    qDebug()<<"[download file] finished";
}
