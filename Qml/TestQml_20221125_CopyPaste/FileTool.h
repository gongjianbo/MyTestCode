#pragma once
#include <QObject>
#include <QUrl>

//文件相关操作
class FileTool : public QObject
{
    Q_OBJECT
public:
    explicit FileTool(QObject *parent = nullptr);

    //判断url是否为QImage可解析的图片文件
    Q_INVOKABLE bool isImageFile(const QUrl &url);
    //复制图片到某个路径，demo直接copy到exe旁的image文件夹
    Q_INVOKABLE QUrl copyImage(const QUrl &url);
    //判断url是否有效
    Q_INVOKABLE bool urlValid(const QUrl &url);
};
