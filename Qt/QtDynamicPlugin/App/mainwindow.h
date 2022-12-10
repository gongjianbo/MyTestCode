#pragma once
#include <QMainWindow>
#include <QVBoxLayout>

class MainWindow : public QMainWindow
{
    Q_OBJECT
public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

    void loadPlugin();

signals:
    void mainSignal(const QString &arg);

public slots:
    void mainSlot(const QString &arg);

private:
    QVBoxLayout *layout{nullptr};
};
