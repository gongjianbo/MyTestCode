#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QCoreApplication>
#include <QFile>
#include <QDir>
#include <QFileInfo>
#include <QTextCodec>
#include <QSettings>
#include <QDebug>
#include <Windows.h>
#include "uchardet.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    // 遍历文件，放到combobox列表
    QDir dir(getIniDir());
    const QFileInfoList list = dir.entryInfoList(
        QStringList() << "*.ini",
        QDir::NoSymLinks | QDir::NoDotAndDotDot | QDir::Files);
    for (auto &&info : list){
        ui->comboBox->addItem(info.fileName());
    }
}

MainWindow::~MainWindow()
{
    delete ui;
}

QString MainWindow::getIniDir()
{
    return QFileInfo(QCoreApplication::applicationDirPath() + "/../ini").absoluteFilePath();
}

QString MainWindow::getIniPath(const QString &filename)
{
    return QFileInfo(QCoreApplication::applicationDirPath() + "/../ini/" + filename).absoluteFilePath();
}

QString MainWindow::myCodec(const QString &filepath)
{
    const QString default_codec = "utf8";

    QFile file(filepath);
    if (file.size() > 0 && file.open(QIODevice::ReadOnly)) {
        // file释放时自动close
        QByteArray buf = file.readLine(4);
        quint8 b1, b2, b3;
        if (buf.size() < 2) {
            return default_codec;
        } else {
            b1 = buf.at(0);
            b2 = buf.at(1);
        }
        if (b1 == 0xFF && b2 == 0xFE) {
            return "utf-16le";
        } else if (b1 == 0xFE && b2 == 0xFF) {
            return "utf-16be";
        }
        if (buf.size() < 3) {
            return default_codec;
        } else {
            b3 = buf.at(2);
        }
        if (b1 == 0xEF && b2 == 0xBB && b3 == 0xBF) {
            // utf-8带bom
            return "utf-8";
        } else {
            // 对于不带bom的文件需要全文检测
            file.seek(0);
            buf = file.readAll();
            // 尝试用utf8转换，如果无效字符数大于0，则非utf8编码
            QTextCodec::ConverterState state;
            QTextCodec *codec = QTextCodec::codecForName("utf-8");
            codec->toUnicode(buf.constData(), buf.size(), &state);
            if (state.invalidChars > 0) {
                return "system";
            }
        }
    }
    return default_codec;
}

QString MainWindow::uchardetCodec(const QString &filepath)
{
    QFile file(filepath);
    if (file.open(QIODevice::ReadOnly)) {
        QByteArray buf = file.readAll();
        file.close();
        uchardet_t ud = uchardet_new();
        uchardet_handle_data(ud, buf.constData(), buf.size());
        uchardet_data_end(ud);
        const char* encoding = uchardet_get_charset(ud);
        QString result = QString::fromLatin1(encoding);
        uchardet_delete(ud);
        return result;
    }
    return "utf-8";
}

void MainWindow::on_myTest_clicked()
{
    const QString filepath = getIniPath(ui->comboBox->currentText());
    const QByteArray codecname = myCodec(filepath).toLatin1();
    ui->textEdit->append("Test File: " + filepath);
    ui->textEdit->append("Codec: " + codecname);
    ui->textEdit->append("Content: ");
    QFile file(filepath);
    if (file.open(QIODevice::ReadOnly)) {
        QByteArray buf = file.readAll();
        file.close();
        QTextCodec::ConverterState state;
        QTextCodec *codec = QTextCodec::codecForName(codecname);
        QString str = codec->toUnicode(buf.constData(), buf.size(), &state);
        ui->textEdit->append(str);
    }
}

void MainWindow::on_myRead_clicked()
{
    const QString filepath = getIniPath(ui->comboBox->currentText());
    const QByteArray codecname = myCodec(filepath).toLatin1();
    // 实测QSettings读写gbk或utf8编码正常，utf16不支持，不支持的编码可以registerFormat自定义编解码
    QSettings setting(filepath, QSettings::IniFormat);
    setting.setIniCodec(codecname.data());
    ui->textEdit->append("Read En: " + setting.value("Config/En", "Default").toString());
    ui->textEdit->append("Read Zh: " + setting.value("Config/Zh", "Default").toString());
    ui->textEdit->append("Read Date: " + QString::number(setting.value("Config/Date", 0).toLongLong()));
}

void MainWindow::on_myWrite_clicked()
{
    const QString filepath = getIniPath(ui->comboBox->currentText());
    const QByteArray codecname = myCodec(filepath).toLatin1();
    // 实测QSettings读写gbk或utf8编码正常，utf16不支持，不支持的编码可以registerFormat自定义编解码
    QSettings setting(getIniPath("write.ini"), QSettings::IniFormat);
    setting.setIniCodec(codecname.data());
    ui->textEdit->append("Write to write.ini");
    setting.setValue("Config/En", "English");
    setting.setValue("Config/Zh", "中文测试");
    setting.setValue("Config/Date", 20250320);
}

void MainWindow::on_uchardetTest_clicked()
{
    const QString filepath = getIniPath(ui->comboBox->currentText());
    const QByteArray codecname = uchardetCodec(filepath).toLatin1();
    ui->textEdit->append("Test File: " + filepath);
    ui->textEdit->append("Codec: " + codecname);
    ui->textEdit->append("Content: ");
    QFile file(filepath);
    if (file.open(QIODevice::ReadOnly)) {
        QByteArray buf = file.readAll();
        file.close();
        QTextCodec::ConverterState state;
        QTextCodec *codec = QTextCodec::codecForName(codecname);
        if (!codec) {
            qDebug() << "unsupport" << codecname;
            return;
        }
        // toUnicode有时候会崩掉，不知道是因为编码不对导致的还是啥
        QString str = codec->toUnicode(buf.constData(), buf.size(), &state);
        ui->textEdit->append(str);
    }
}

void MainWindow::on_profileRead_clicked()
{
    std::wstring filepath = getIniPath(ui->comboBox->currentText()).toStdWString();
    // 实测win32这个接口可以读写utf16le和系统编码（如gbk），其他不支持
    wchar_t temp[1024] = {0};
    GetPrivateProfileStringW(L"Config", L"En", L"Default", temp, sizeof(temp), filepath.c_str());
    ui->textEdit->append("Read En: " + QString::fromStdWString(std::wstring(temp)));
    memset(temp, 0, sizeof(temp));
    GetPrivateProfileStringW(L"Config", L"Zh", L"Default", temp, sizeof(temp), filepath.c_str());
    ui->textEdit->append("Read Zh: " + QString::fromStdWString(std::wstring(temp)));
    memset(temp, 0, sizeof(temp));
    uint32_t date = GetPrivateProfileIntW(L"Config", L"Date", 0, filepath.c_str());
    ui->textEdit->append("Read Date: " + QString::number(date));
}

void MainWindow::on_profileWrite_clicked()
{
    std::wstring filepath = getIniPath("write.ini").toStdWString();
    // 写默认是系统编码（如gbk），如果文件是utf16le则使用文件的编码
    WritePrivateProfileStringW(L"Config", L"En", L"English", filepath.c_str());
    WritePrivateProfileStringW(L"Config", L"Zh", L"中文测试", filepath.c_str());
    WritePrivateProfileStringW(L"Config", L"Date", L"20250320", filepath.c_str());
}
