#ifndef MAINWIDGET_H
#define MAINWIDGET_H

#include <QWidget>

// QT += network
#include <QNetworkAccessManager>
#include <QNetworkRequest>
#include <QNetworkReply>
#include <QHttpPart>
#include <QHttpMultiPart>
#include <QFile>

#include <QTimer>

namespace Ui {
class MainWidget;
}

class MainWidget : public QWidget
{
    Q_OBJECT

public:
    explicit MainWidget(QWidget *parent = nullptr);
    ~MainWidget();

    void getTest();
    void postTest();
    void formTest();

public slots:
    void receiveReply(QNetworkReply *reply);

private:
    Ui::MainWidget *ui;

    QNetworkAccessManager *manager;
    QTimer timer;
};

#endif // MAINWIDGET_H
