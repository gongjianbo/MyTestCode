#include "ClipboardTool.h"
#include <QGuiApplication>
#include <QClipboard>
#include <QDebug>

ClipboardTool::ClipboardTool(QObject *parent)
    : QObject{parent}
{

}

QString ClipboardTool::currentText()
{
    QClipboard *clipboard = QGuiApplication::clipboard();
    if (clipboard) {
        return clipboard->text();
    }
    return QString();
}

QUrl ClipboardTool::currentUrl()
{
    return QUrl(currentText());
}

void ClipboardTool::setCurrentUrl(const QUrl &url)
{
    QClipboard *clipboard = QGuiApplication::clipboard();
    if (clipboard) {
        //QClipboard内部会调用QMimeData::setText
        //本来以为以为要自己设置mimedata的image，但是测试发现setText后ctrl+v粘贴就能把图片粘贴出去
        clipboard->setText(url.toString());
    }
}
