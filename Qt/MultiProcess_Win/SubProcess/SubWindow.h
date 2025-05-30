#pragma once
#include <QMainWindow>

QT_BEGIN_NAMESPACE
namespace Ui { class SubWindow; }
QT_END_NAMESPACE

class SubWindow : public QMainWindow
{
    Q_OBJECT
public:
    explicit SubWindow(QWidget *parent = nullptr);
    ~SubWindow();

private:
    Ui::SubWindow *ui;
};

