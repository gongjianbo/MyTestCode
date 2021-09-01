#include "VoicePrintSource.h"
#include <QDebug>

VoicePrintSource::VoicePrintSource(QObject *parent)
    : LocalNode(parent)
{

}

void VoicePrintSource::appendFile(const QStringList &filelist)
{
    qDebug()<<"append file"<<filelist;
}

void VoicePrintSource::editFile(const QString &filepath)
{
    qDebug()<<"edit file"<<filepath;
}

void VoicePrintSource::removeFile(const QStringList &filelist)
{
    qDebug()<<"remove file"<<filelist;
}

void VoicePrintSource::requestExtractProgress()
{
    qDebug()<<"request extract progress";
}

void VoicePrintSource::compareRun(const QStringList &materials, const QStringList &samples, int algorithm)
{
    qDebug()<<"compare run"<<materials<<samples<<algorithm;
}

void VoicePrintSource::clusterRun(const QStringList &filelist, int taskId)
{
    qDebug()<<"clister run"<<filelist<<taskId;
}

void VoicePrintSource::updateExtractProgress(int extract, int unextract)
{
    QByteArray data;
    QDataStream stream(&data, QIODevice::ReadWrite);
    stream<<extract<<unextract;
    sendData(LMEType::Feature, LMEFeature::ExtractProgress, data);
}

void VoicePrintSource::updateCompareProgress(int progress)
{
    QByteArray data;
    QDataStream stream(&data, QIODevice::ReadWrite);
    stream<<progress;
    sendData(LMEType::Compare, LMETask::Progress, data);
}

void VoicePrintSource::compareResult(bool result)
{
    QByteArray data;
    QDataStream stream(&data, QIODevice::ReadWrite);
    stream<<result;
    sendData(LMEType::Compare, LMETask::Response, data);
}

void VoicePrintSource::updateClusterProgress(int progress)
{
    QByteArray data;
    QDataStream stream(&data, QIODevice::ReadWrite);
    stream<<progress;
    sendData(LMEType::Cluster, LMETask::Progress, data);
}

void VoicePrintSource::clusterResult(bool result)
{
    QByteArray data;
    QDataStream stream(&data, QIODevice::ReadWrite);
    stream<<result;
    sendData(LMEType::Cluster, LMETask::Response, data);
}

void VoicePrintSource::netDisconnected()
{
    emit errorChanged();
    qDebug()<<"voice print socket disconnected.";
}

void VoicePrintSource::recvData(quint8 major, quint8 minor, const QByteArray &data)
{
    QDataStream stream(data);
    switch(major) {
    case LMEType::Feature: //特征提取
    {
        switch (minor) {
        case LMEFeature::AppendFile:
        {
            QStringList filelist;
            stream>>filelist;
            appendFile(filelist);
        }
            break;
        case LMEFeature::EditFile:
        {
            QString filepath;
            stream>>filepath;
            editFile(filepath);
        }
            break;
        case LMEFeature::RemoveFile:
        {
            QStringList filelist;
            stream>>filelist;
            removeFile(filelist);
        }
            break;
        case LMEFeature::RequestProgress:
        {
            requestExtractProgress();
        }
            break;
        }
    }
        break;
    case LMEType::Compare: //比对
    {
        switch (minor) {
        case LMETask::Request:
        {
            QStringList materials;
            QStringList samples;
            int algorithm;
            stream>>materials>>samples>>algorithm;
            compareRun(materials,samples,algorithm);
        }
            break;
        }
    }
        break;
    case LMEType::Cluster: //聚类
    {
        switch (minor) {
        case LMETask::Request:
        {
            QStringList filelist;
            int taskId;
            stream>>filelist>>taskId;
            clusterRun(filelist,taskId);
        }
            break;
        }
    }
        break;
    }
}
