#include "VoicePrintClient.h"
#include <QDebug>

VoicePrintClient::VoicePrintClient(QObject *parent)
    : LocalNode(parent)
{

}

void VoicePrintClient::appendFile(const QStringList &filelist)
{
    QByteArray data;
    QDataStream stream(&data, QIODevice::ReadWrite);
    stream<<filelist;
    sendData(LMEType::Feature, LMEFeature::AppendFile, data);
}

void VoicePrintClient::editFile(const QString &filepath)
{
    QByteArray data;
    QDataStream stream(&data, QIODevice::ReadWrite);
    stream<<filepath;
    sendData(LMEType::Feature, LMEFeature::EditFile, data);
}

void VoicePrintClient::removeFile(const QStringList &filelist)
{
    QByteArray data;
    QDataStream stream(&data, QIODevice::ReadWrite);
    stream<<filelist;
    sendData(LMEType::Feature, LMEFeature::RemoveFile, data);
}

void VoicePrintClient::requestExtractProgress()
{
    sendData(LMEType::Feature, LMEFeature::RequestProgress, QByteArray());
}

void VoicePrintClient::compareRun(const QStringList &materials, const QStringList &samples, int algorithm)
{
    QByteArray data;
    QDataStream stream(&data, QIODevice::ReadWrite);
    stream<<materials<<samples<<algorithm;

    //比对请求
    if(!sendData(LMEType::Compare, LMETask::Request, data)){
        emit errorChanged();
    }
}

void VoicePrintClient::compareStop()
{
    qDebug()<<"compare stop";
    //断开连接
    nodeAbort();
}

void VoicePrintClient::clusterRun(const QStringList &filelist, int taskId)
{
    QByteArray data;
    QDataStream stream(&data, QIODevice::ReadWrite);
    stream<<filelist<<taskId;

    //聚类请求
    if(!sendData(LMEType::Cluster, LMETask::Request, data)){
        emit errorChanged();
    }
}

void VoicePrintClient::clusterStop()
{
    qDebug()<<"cluster stop";
    //断开连接
    nodeAbort();
}

void VoicePrintClient::init()
{
    nodeInit(VoicePrint_Server);
}

void VoicePrintClient::free()
{
    nodeFree();
}

void VoicePrintClient::netConnected()
{
    LocalNode::netConnected();
    qDebug()<<"voice print socket connected.";
}

void VoicePrintClient::netDisconnected()
{
    LocalNode::netDisconnected();
    emit errorChanged();
    qDebug()<<"voice print socket disconnected.";
}

void VoicePrintClient::recvData(quint8 major, quint8 minor, const QByteArray &data)
{
    QDataStream stream(data);
    switch(major) {
    case LMEType::Feature: //特征提取
    {
        switch (minor) {
        case LMEFeature::ExtractProgress:
        {
            int extract,unextract;
            stream>>extract>>unextract;
            emit extractProgressChanged(extract,unextract);
        }
            break;
        }
    }
        break;
    case LMEType::Compare: //比对
    {
        switch (minor) {
        case LMETask::Response:
        {
            bool ret;
            stream>>ret;
            emit compareFinished(ret);
        }
            break;
        case LMETask::Progress:
        {
            int progress;
            stream>>progress;
            emit compareProgressChanged(progress);
        }
            break;
        }
    }
        break;
    case LMEType::Cluster: //聚类
    {
        switch (minor) {
        case LMETask::Response:
        {
            bool ret;
            stream>>ret;
            emit clusterFinished(ret);
        }
            break;
        case LMETask::Progress:
        {
            int progress;
            stream>>progress;
            emit clusterProgressChanged(progress);
        }
            break;
        }
    }
        break;
    }
}
