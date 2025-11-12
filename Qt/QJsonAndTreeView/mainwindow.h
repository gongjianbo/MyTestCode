#pragma once

#include <QMainWindow>

// #include "ParseJson.h"
#include "JsonTreeModel.h"

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT
public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private:
    void initLoadDump();
    void initEdit();
    void updateIndex();

private:
    Ui::MainWindow *ui;
    // ParseJson parseJson;
    JsonTreeModel *jsonModel;
};
