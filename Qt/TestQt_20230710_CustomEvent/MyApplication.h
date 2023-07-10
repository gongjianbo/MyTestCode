#pragma once
#include <QApplication>

class MyApplication : public QApplication
{
    Q_OBJECT
public:
    MyApplication(int &argc, char **argv, int = QApplication::ApplicationFlags);

protected:
    // 事件合并判
    bool compressEvent(QEvent *, QObject *receiver, QPostEventList *) override;
};
