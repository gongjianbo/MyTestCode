#include "FileDialogWrap.h"
#include <QCoreApplication>
#include <QStandardPaths>
#include <QFileInfo>
#include <QDebug>

FileDialogWrap::FileDialogWrap(QObject *parent)
    : QObject{parent}
    , dialog{new QFileDialog}
{
    setTitle(QCoreApplication::applicationName());
    connect(this, &FileDialogWrap::__openRequest, this, &FileDialogWrap::openDialog, Qt::QueuedConnection);
    connect(this, &FileDialogWrap::__closeRequest, this, [=](){
        dialog->reject();
    }, Qt::QueuedConnection);
}

FileDialogWrap::~FileDialogWrap()
{
    dialog->deleteLater();
}

int FileDialogWrap::getMode() const
{
    return workMode;
}

void FileDialogWrap::setMode(int mode)
{
    if (mode != workMode) {
        workMode = mode;
        emit modeChanged();
    }
}

QString FileDialogWrap::getTitle() const
{
    return dialog->windowTitle();
}

void FileDialogWrap::setTitle(const QString &title)
{
    if (title != getTitle()) {
        dialog->setWindowTitle(title);
        emit titleChanged();
    }
}

QStringList FileDialogWrap::getNameFilters() const
{
    return nameFilters;
}

void FileDialogWrap::setNameFilters(const QStringList &filters)
{
    if (filters != nameFilters) {
        nameFilters = filters;
        emit nameFiltersChanged();
    }
}

QUrl FileDialogWrap::getCurrentUrl() const
{
    return currentUrl;
}

void FileDialogWrap::setCurrentUrl(const QUrl &url)
{
    if (url != currentUrl) {
        currentUrl = url;
        emit currentUrlChanged();
    }
}

QList<QUrl> FileDialogWrap::getCurrentUrls() const
{
    return currentUrls;
}

void FileDialogWrap::setCurrentUrls(const QList<QUrl> &urls)
{
    if (urls != currentUrls) {
        currentUrls = urls;
        emit currentUrlsChanged();
    }
}

void FileDialogWrap::open()
{
    if (dialog->isVisible())
        return;
    emit __openRequest();
}

void FileDialogWrap::close()
{
    if (!dialog->isVisible())
        return;
    emit __closeRequest();
}

void FileDialogWrap::selectUrl(const QUrl &url)
{
    nextUrl = url;
}

QString FileDialogWrap::desktopPath()
{
    return QStandardPaths::writableLocation(QStandardPaths::DesktopLocation);
}

QUrl FileDialogWrap::desktopUrl()
{
    return QUrl::fromLocalFile(desktopPath());
}

void FileDialogWrap::initSetting(const QString &filepath, const QSettings::Format &format)
{
    iniPath = filepath;
    iniFormat = format;
}

QString FileDialogWrap::getDefaultPath()
{
    //QFileDialog默认实现在构造时恢复，析构时保存
    //QSettings settings(QSettings::UserScope, QLatin1String("QtProject"));
    //settings.beginGroup(QLatin1String("FileDialog"));
    //记录上一次选择的路径
    QSettings setting(iniPath, QSettings::IniFormat);
    QString dirpath = setting.value("LastPath").toString();
    if (dirpath.isEmpty()) {
        //默认桌面路径
        dirpath = QStandardPaths::writableLocation(QStandardPaths::DesktopLocation);
    }
    qDebug()<<"get default"<<dirpath;
    return dirpath;
}

void FileDialogWrap::setDefaultPath(const QString &dirpath)
{
    QSettings setting(iniPath, QSettings::IniFormat);
    setting.setValue("LastPath", dirpath);
    qDebug()<<"set default"<<dirpath;
}

void FileDialogWrap::openDialog()
{
    switch (workMode) {
    case SelectFile:
    {
        dialog->setFileMode(QFileDialog::ExistingFile);
    }
        break;
    case SelectFiles:
    {
        dialog->setFileMode(QFileDialog::ExistingFiles);
    }
        break;
    case SelectDir:
    {
        dialog->setOption(QFileDialog::ShowDirsOnly);
        //参考源码中的写法
        QT_WARNING_PUSH
        QT_WARNING_DISABLE_DEPRECATED
        dialog->setFileMode(QFileDialog::DirectoryOnly);
        QT_WARNING_POP

    }
        break;
    case SaveFile:
    {
        dialog->setFileMode(QFileDialog::AnyFile);
        dialog->setAcceptMode(QFileDialog::AcceptSave);
    }
        break;
    default: break;
    }

    dialog->setNameFilters(nameFilters);
    //设置了url就用设置的，否则用上次保存的目录
    //selectUrl设置nextUrl
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
    auto &&urls = dialog->selectedUrls();
    if (urls.isEmpty()) {
        setCurrentUrl(QUrl());
        setDefaultPath(desktopPath());
    } else {
        QUrl first_url = urls.first();
        setCurrentUrl(first_url);
        //保存目录到配置文件
        QFileInfo info(first_url.toLocalFile());
        if (info.isDir()) {
            setDefaultPath(info.absoluteFilePath());
        } else {
            setDefaultPath(info.absolutePath());
        }
    }
    setCurrentUrls(urls);
    emit accepted();
}
