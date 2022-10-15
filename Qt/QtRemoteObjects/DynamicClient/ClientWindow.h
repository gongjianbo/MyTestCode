#pragma once
#include <QMainWindow>
#include <QRemoteObjectNode>
#include <QRemoteObjectDynamicReplica>

QT_BEGIN_NAMESPACE
namespace Ui { class ClientWindow; }
QT_END_NAMESPACE

class ClientWindow : public QMainWindow
{
    Q_OBJECT
public:
    ClientWindow(QWidget *parent = nullptr);
    ~ClientWindow();

public slots:
    void initConnection();
    void onDataChange(const QString &str);

private:
    Ui::ClientWindow *ui;

    QRemoteObjectNode remoteNode;
    QSharedPointer<QRemoteObjectDynamicReplica> replica;
};
