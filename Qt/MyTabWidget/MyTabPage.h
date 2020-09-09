#ifndef MYTABPAGE_H
#define MYTABPAGE_H

#include <QWidget>

/**
 * @brief 单个Page页
 * @details 用于测试构造析构等
 */
class MyTabPage : public QWidget
{
    Q_OBJECT
public:
    explicit MyTabPage(QWidget *parent = nullptr);
    ~MyTabPage();

    //获取唯一id
    int getId() const { return id; }

private:
    int id=0;
};

#endif // MYTABPAGE_H
