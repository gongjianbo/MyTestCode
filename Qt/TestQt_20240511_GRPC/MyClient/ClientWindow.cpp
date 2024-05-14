#include "ClientWindow.h"
#include "ui_ClientWindow.h"
#include <QFileDialog>
#include <QDateTime>
#include <QDebug>

ClientWindow::ClientWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::ClientWindow)
{
    ui->setupUi(this);
    connect(&node, &MyNode::updateLog, ui->editLog, &QTextEdit::append);
    connect(ui->btnLogin, &QPushButton::clicked, this, &ClientWindow::login);
    connect(ui->btnLogout, &QPushButton::clicked, this, &ClientWindow::logout);
    connect(ui->btnSend, &QPushButton::clicked, this, &ClientWindow::sendRequest);
    connect(ui->btnPath, &QPushButton::clicked, this, &ClientWindow::selectImage);
}

ClientWindow::~ClientWindow()
{
    delete ui;
    node.logout();
}

void ClientWindow::login()
{
    node.login(ui->editHost->text());
}

void ClientWindow::logout()
{
    node.logout();
}

void ClientWindow::sendRequest()
{
    qDebug()<<"begin send"<<QTime::currentTime();
    switch (ui->comboMessage->currentIndex() + MyPackage::InsertImageMsg)
    {
    case MyPackage::InsertImageMsg:
        node.insertImage(ui->editPath->text());
        break;
    case MyPackage::DeleteImageMsg:
        node.deleteImage();
        break;
    case MyPackage::UpdateImageMsg:
        node.updateImage();
        break;
    case MyPackage::SelectImageMsg:
        node.selectImage();
        break;
    }
    qDebug()<<"end send"<<QTime::currentTime();
}

void ClientWindow::selectImage()
{
    QString path = QFileDialog::getOpenFileName(this, "Select Image", "", "PNG (*.png)");
    if (path.isEmpty())
        return;
    ui->editPath->setText(path);
}
