#pragma once
#include "LocalNode.h"

//接口调用--服务端部分
class VoicePrintSource : public LocalNode
{
    Q_OBJECT
public:
    explicit VoicePrintSource(QObject *parent = nullptr);

    //【】特征提取
    //新增文件
    void appendFile(const QStringList &filelist);
    //修改文件
    void editFile(const QString &filepath);
    //移除文件
    void removeFile(const QStringList &filelist);
    //请求发送进度过来
    void requestExtractProgress();
    //【】比对
    //开始比对
    void compareRun(const QStringList &materials, const QStringList &samples, int algorithm);
    //【】聚类
    //开始聚类
    void clusterRun(const QStringList &filelist, int taskId);
    //【】反馈给客户端的信息
    //特征提取进度
    void updateExtractProgress(int extract, int unextract);
    //比对进度
    void updateCompareProgress(int progress);
    //比对结束
    void compareResult(bool result);
    //聚类进度
    void updateClusterProgress(int progress);
    //聚类结束
    void clusterResult(bool result);

signals:
    //发送失败，网络异常等，此时所有任务标记为失败
    void errorChanged();

protected slots:
    //
    //void init();
    //void free();
    //socket连接成功
    //void netConnected() override;
    //socket断开连接
    void netDisconnected() override;
    //数据接收
    void recvData(quint8 major, quint8 minor, const QByteArray &data) override;
};


