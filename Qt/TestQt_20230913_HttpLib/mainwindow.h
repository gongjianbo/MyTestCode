#pragma once
#include <QMainWindow>
#include <string>
#include "httplib.h"

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT
public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

    httplib::Client getClient() const;
    std::string getPath(const std::string &method) const;
    std::string jsonToUri(const std::string &json) const;
    void printResult(const httplib::Result &res) const;

public slots:
    // 测试不同的请求方式
    void doGet();
    void doPost();
    void doPut();
    void doPatch();
    void doDelete();
    // 测试文件上传下载
    void doUp();
    void doDown();

private:
    Ui::MainWindow *ui;
};
