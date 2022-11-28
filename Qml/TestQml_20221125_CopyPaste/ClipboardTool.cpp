#include "ClipboardTool.h"
#include <QGuiApplication>
#include <QClipboard>
#include <QMimeData>
#include <QDebug>

ClipboardTool::ClipboardTool(QObject *parent)
    : QObject{parent}
{

}

QUrl ClipboardTool::currentUrl()
{
    QClipboard *clipboard = QGuiApplication::clipboard();
    if (clipboard) {
        auto md = clipboard->mimeData();
        if (md) {
            auto urls = md->urls();
            if (!urls.isEmpty()) {
                return urls.first();
            }
        }
    }
    return QUrl();
}

void ClipboardTool::setCurrentUrl(const QUrl &url)
{
    QClipboard *clipboard = QGuiApplication::clipboard();
    if (clipboard) {
        QMimeData *md = new QMimeData;
        md->setUrls(QList<QUrl>()<<url);
        clipboard->setMimeData(md);
    }
}
