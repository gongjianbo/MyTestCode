#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QCoreApplication>
#include <QSettings>
#include <QDateTime>
#include <QFileInfo>
#include <QMetaType>
#include <QDebug>
#include "CustomSettings.h"
#include "EasyIni.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    // int值或者bool字符串可以转bool，但是bool字符串转数值都是0
    // qDebug() << QVariant("0").toBool() << QVariant("1").toBool();
    // qDebug() << QVariant("false").toBool() << QVariant("true").toBool();
    // qDebug() << QVariant("false").toInt() << QVariant("true").toInt();
    // qDebug() << QVariant("false").toDouble() << QVariant("true").toDouble();
}

MainWindow::~MainWindow()
{
    delete ui;
}

QString MainWindow::getIniPath(const QString &filename)
{
    return QFileInfo(QCoreApplication::applicationDirPath() + "/../ini/" + filename).absoluteFilePath();
}

void MainWindow::on_qtWrite_clicked()
{
    const QString path = getIniPath(ui->qtEdit->text());
    qDebug() << __FUNCTION__ << path;
    QSettings setting(path, QSettings::IniFormat);
    setting.setIniCodec("UTF-8");
    setting.setValue("BB/AA", "测试");
    setting.setValue("BB/BB", true);
    setting.setValue("BB/CC", 123.7);
    setting.setValue("CC/Byte", QByteArray::fromHex("304161"));
    setting.setValue("CC/Size", QSize(10, 20));
    setting.setValue("CC/StringList", QStringList() << "A" << "B" << "C");
    setting.setValue("CC/VariantList", QVariantList() << QSize(10, 20) << 1992 << QByteArray("AA"));
    // 保存ini会把中文写成unicode："%U4E2D%U6587"
    // 但是读取ini时，手动输入的中文是不能正常读取的，
    // 且保存ini的时候会修改掉手写中文的格式，如中文变成"%E4%B8%AD%E6%96%87"
    setting.setValue("中文/中文", "测试中文");
}

void MainWindow::on_qtRead_clicked()
{
    const QString path = getIniPath(ui->qtEdit->text());
    qDebug() << __FUNCTION__ << path;
    QFileInfo info(path);
    qDebug() << info.lastModified() << info.metadataChangeTime();

    QSettings setting(path, QSettings::IniFormat);
    setting.setIniCodec("UTF-8");
    qDebug() << "keys" << setting.allKeys();
    qDebug() << setting.value("BB/AA").toString();
    qDebug() << setting.value("BB/BB").toBool();
    qDebug() << setting.value("BB/CC").toDouble();
    qDebug() << setting.value("CC/Byte").toByteArray().toHex().toUpper();
    qDebug() << setting.value("CC/Size").toSize();
    qDebug() << setting.value("CC/StringList").toStringList();
    qDebug() << setting.value("CC/VariantList").toList();
    qDebug() << setting.value("中文/中文");
    qDebug() << setting.value("None/AA");
}

void MainWindow::on_customWrite_clicked()
{
    const QString path = getIniPath(ui->customEdit->text());
    qDebug() << __FUNCTION__ << path;
    CustomSettings setting(path);
    setting.setValue("BB/AA", "测试");
    setting.setValue("BB/BB", true);
    setting.setValue("BB/CC", 123.7);
    setting.setValue("CC/Byte", QByteArray::fromHex("304161"));
    setting.setValue("CC/Size", QSize(10, 20));
    setting.setValue("CC/StringList", QStringList() << "A" << "B" << "C");
    // VariantList暂时用不到就不做额外处理了，统一作为StringList
    // setting.setValue("CC/VariantList", QVariantList() << QSize(10, 20) << 1992 << QByteArray("AA"));
    setting.setValue("中文/中文", "测试中文");
}

void MainWindow::on_customRead_clicked()
{
    const QString path = getIniPath(ui->customEdit->text());
    qDebug() << __FUNCTION__ << path;
    QFileInfo info(path);
    qDebug() << info.lastModified() << info.metadataChangeTime();

    CustomSettings setting(path);
    qDebug() << "keys" << setting.allKeys();
    qDebug() << setting.value("BB/AA").toString();
    qDebug() << setting.value("BB/BB").toBool();
    qDebug() << setting.value("BB/CC").toDouble();
    qDebug() << setting.value("CC/Byte").toByteArray().toHex().toUpper();
    qDebug() << setting.value("CC/Size").toSize();
    qDebug() << setting.value("CC/StringList").toStringList();
    // qDebug() << setting.value("CC/VariantList").toList();
    qDebug() << setting.value("中文/中文");
    qDebug() << setting.value("None/AA");
}

void MainWindow::on_easyWrite_clicked()
{
    using namespace Tool;
    const QString path = getIniPath(ui->easyEdit->text());
    qDebug() << __FUNCTION__ << path;
    EasyIni ini(path, EasyIni::WriteOnly);
    ini.setValue("AA", "AA", "测试");
    ini.setValue("BB", "AA", "测试");
    ini.setValue("BB", "BB", 1);
    ini.setValue("BB", "CC", true);
    ini.setValue("中文组", "中文键", "中文值");
}

void MainWindow::on_easyRead_clicked()
{
    using namespace Tool;
    const QString path = getIniPath(ui->easyEdit->text());
    qDebug() << __FUNCTION__ << path;
    EasyIni ini(path, EasyIni::ReadOnly);
    ini.dumpInfo();
    qDebug() << ini.groups();
    qDebug() << ini.keys("AA");
    qDebug() << ini.keys("A");
    qDebug() << ini.value("BB", "AA");
    qDebug() << ini.value("BB", "BB");
    qDebug() << ini.value("BB", "CC");
    qDebug() << ini.value("BB", "QWER");
    ini.setValue("BB", "CC", true);
    ini.setValue("BB", "QWER", 12345);
    qDebug() << ini.value("BB", "CC");
    qDebug() << ini.value("BB", "QWER");
    qDebug() << ini.value("BB", "QWER2");
}
