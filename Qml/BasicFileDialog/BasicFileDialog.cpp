#include "BasicFileDialog.h"
#include <QCoreApplication>
#include <QSettings>
#include <QFileDialog>
#include <QStandardPaths>
#include <QFileInfo>
#include <QFile>
#include <QDebug>

BasicFileDialog::BasicFileDialog(QObject *parent)
    : QObject{parent}
    , dialog{new QFileDialog}
{
    setTitle(QCoreApplication::applicationName());
    connect(this, &BasicFileDialog::__openRequest, this, &BasicFileDialog::openDialog, Qt::QueuedConnection);
    connect(this, &BasicFileDialog::__closeRequest, this, [=](){
        dialog->reject();
    }, Qt::QueuedConnection);
    connect(this, &BasicFileDialog::accepted, this, &BasicFileDialog::closing, Qt::QueuedConnection);
    connect(this, &BasicFileDialog::rejected, this, &BasicFileDialog::closing, Qt::QueuedConnection);
}

BasicFileDialog::~BasicFileDialog()
{
    dialog->deleteLater();
}

int BasicFileDialog::getMode() const
{
    return workMode;
}

void BasicFileDialog::setMode(int mode)
{
    if (mode != workMode) {
        workMode = mode;
        emit modeChanged();
    }
}

QString BasicFileDialog::getTitle() const
{
    return dialog->windowTitle();
}

void BasicFileDialog::setTitle(const QString &title)
{
    if (title != getTitle()) {
        dialog->setWindowTitle(title);
        emit titleChanged();
    }
}

QStringList BasicFileDialog::getNameFilters() const
{
    return nameFilters;
}

void BasicFileDialog::setNameFilters(const QStringList &filters)
{
    if (filters != nameFilters) {
        nameFilters = filters;
        emit nameFiltersChanged();
    }
}

QString BasicFileDialog::getSelectedNameFilter() const
{
    return selectedNameFilter;
}

void BasicFileDialog::setSelectedNameFilter(const QString &filter)
{
    if (filter != selectedNameFilter) {
        selectedNameFilter = filter;
        emit selectedNameFilterChanged();
    }
}

QUrl BasicFileDialog::getCurrentUrl() const
{
    return currentUrl;
}

void BasicFileDialog::setCurrentUrl(const QUrl &url)
{
    if (url != currentUrl) {
        currentUrl = url;
        emit currentUrlChanged();
    }
}

QList<QUrl> BasicFileDialog::getCurrentUrls() const
{
    return currentUrls;
}

void BasicFileDialog::setCurrentUrls(const QList<QUrl> &urls)
{
    if (urls != currentUrls) {
        currentUrls = urls;
        emit currentUrlsChanged();
    }
}

void BasicFileDialog::show()
{
    open();
}

void BasicFileDialog::open()
{
    if (dialog->isVisible())
        return;
    emit __openRequest();
}

void BasicFileDialog::close()
{
    if (!dialog->isVisible())
        return;
    emit __closeRequest();
}

void BasicFileDialog::selectUrl(const QUrl &url)
{
    nextUrl = url;
}

void BasicFileDialog::selectPath(const QString &path)
{
    nextUrl = QUrl::fromLocalFile(path);
}

QString BasicFileDialog::desktopPath()
{
    return QStandardPaths::writableLocation(QStandardPaths::DesktopLocation);
}

QUrl BasicFileDialog::desktopUrl()
{
    return QUrl::fromLocalFile(desktopPath());
}

QString BasicFileDialog::lastPath()
{
    return getDefaultPath();
}

QUrl BasicFileDialog::lastUrl()
{
    return QUrl::fromLocalFile(lastPath());
}

QString BasicFileDialog::getDefaultPath()
{
    // QFileDialog默认实现在构造时恢复，析构时保存
    // 记录上一次选择的路径
    QSettings setting(QSettings::UserScope, QCoreApplication::applicationName(), "Component");
    setting.setIniCodec("UTF-8");
    QString dirpath = setting.value("FileDialog/LastPath").toString();
    if (dirpath.isEmpty()) {
        // 默认桌面路径
        dirpath = QStandardPaths::writableLocation(QStandardPaths::DesktopLocation);
    }
    // qDebug()<<"get default"<<dirpath;
    return dirpath;
}

void BasicFileDialog::setDefaultPath(const QString &dirpath)
{
    QSettings setting(QSettings::UserScope, QCoreApplication::applicationName(), "Component");
    setting.setIniCodec("UTF-8");
    setting.setValue("FileDialog/LastPath", dirpath);
    // qDebug()<<"set default"<<dirpath;
}

void BasicFileDialog::openDialog()
{
    switch (workMode) {
    case SelectFile: {
        dialog->setFileMode(QFileDialog::ExistingFile);
    } break;
    case SelectFiles: {
        dialog->setFileMode(QFileDialog::ExistingFiles);
    } break;
    case SelectDir: {
#if defined(Q_OS_WIN32)
        dialog->setOption(QFileDialog::ShowDirsOnly);
        // 参考源码中的写法
        QT_WARNING_PUSH
            QT_WARNING_DISABLE_DEPRECATED
                dialog->setFileMode(QFileDialog::DirectoryOnly);
        QT_WARNING_POP
#else
        dialog->setFileMode(QFileDialog::Directory);
        dialog->setOption(QFileDialog::ShowDirsOnly);
#endif
    } break;
    case SaveFile: {
        dialog->setFileMode(QFileDialog::AnyFile);
        dialog->setAcceptMode(QFileDialog::AcceptSave);
    } break;
    default: break;
    }

    dialog->setNameFilters(nameFilters);
    if (!selectedNameFilter.isEmpty()) {
        dialog->selectNameFilter(selectedNameFilter);
    }
    // 设置了url就用设置的，否则用上次保存的目录
    // selectUrl设置nextUrl
    if (nextUrl.isValid()) {
        QFileInfo info(nextUrl.toLocalFile());
        if (info.isDir()) {
            dialog->setDirectory(info.absoluteFilePath());
        } else {
            dialog->setDirectory(info.absolutePath());
            dialog->selectFile(info.fileName());
        }
        nextUrl.clear();
    } else {
        dialog->setDirectory(getDefaultPath());
    }

    const int ret = dialog->exec();
    if (ret == QDialog::Rejected) {
        emit rejected();
        return;
    }
    auto &&paths = dialog->selectedFiles();
    QList<QUrl> urls;
    for (auto &&path : qAsConst(paths))
        urls.push_back(QUrl::fromLocalFile(path));
    if (urls.isEmpty()) {
        setCurrentUrl(QUrl());
        setDefaultPath(desktopPath());
    } else {
        QString first_path = paths.first();
        QFileInfo info(first_path);
        // qDebug()<<__FUNCTION__<<first_path<<info.isFile()<<info.filePath();
        setCurrentUrl(urls.first());
        // 保存目录到配置文件
        if (info.isDir()) {
            setDefaultPath(info.absoluteFilePath());
        } else {
            setDefaultPath(info.absolutePath());
        }
    }
    setCurrentUrls(urls);
    emit accepted();
}
