#ifndef WEBSOCKETCLIENT_H
#define WEBSOCKETCLIENT_H

#include <QWidget>
#include <QWebSocket>

namespace Ui {
class WebSocketClient;
}

//客户端
class WebSocketClient : public QWidget
{
    Q_OBJECT

public:
    explicit WebSocketClient(QWidget *parent = nullptr);
    ~WebSocketClient();

private:
    Ui::WebSocketClient *ui;

    QWebSocket *client;
};

#endif // WEBSOCKETCLIENT_H
