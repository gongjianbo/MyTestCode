#ifndef WEBSOCKETSERVER_H
#define WEBSOCKETSERVER_H

#include <QWidget>
#include <QWebSocketServer>
#include <QWebSocket>

namespace Ui {
class WebSocketServer;
}

//服务端
class WebSocketServer : public QWidget
{
    Q_OBJECT

public:
    explicit WebSocketServer(QWidget *parent = nullptr);
    ~WebSocketServer();

signals:
    void sendMessage(const QString &text);

private:
    void clearClient();
    void onNewConnection();

private:
    Ui::WebSocketServer *ui;

    QWebSocketServer *server;
    QList<QWebSocket*> clientList;
};

#endif // WEBSOCKETSERVER_H
