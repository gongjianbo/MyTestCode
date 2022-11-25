#include "FileTool.h"
#include <QCoreApplication>
#include <QFileInfo>
#include <QFile>
#include <QDir>
#include <QImage>
#include <QImageReader>
#include <QDebug>

FileTool::FileTool(QObject *parent)
    : QObject{parent}
{

}

bool FileTool::isImageFile(const QUrl &url)
{
    if (QFileInfo(url.toLocalFile()).isFile()) {
        return !QImageReader::imageFormat(url.toLocalFile()).isEmpty();
    }
    return false;
}

QUrl FileTool::copyImage(const QUrl &url)
{
    QString dir_path = QCoreApplication::applicationDirPath()+"/image";
    QDir dir(dir_path);
    if (dir.exists() || dir.mkdir(dir_path)) {
        QString src_path = url.toLocalFile();
        QString obj_path = dir_path + "/" + QFileInfo(src_path).completeBaseName() + ".png";

        //名称相同，根据需求可以删除or复用之前的图
        if (QFile::exists(obj_path)) {
            return QUrl::fromLocalFile(obj_path);
        }

        QImage image(src_path);
        if (image.save(obj_path, "png")) {
            return QUrl::fromLocalFile(obj_path);
        }
    }
    return QUrl();
}

bool FileTool::urlValid(const QUrl &url)
{
    return url.isValid();
}
