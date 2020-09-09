#ifndef MYTABWIDGET_H
#define MYTABWIDGET_H

#include <QTabWidget>

/**
 * @brief 自定义TabWidget
 * @details setTabBar为protected接口，要修改tabbar就得继承下
 */
class MyTabWidget : public QTabWidget
{
    Q_OBJECT
public:
    explicit MyTabWidget(QWidget *parent = nullptr);

    //添加一个页
    int appendNormalPage(QWidget *page);
    //移除一个页
    void removeNormalPage(QWidget *page);
    //从tab移出某页，但不close
    void takeNormalPage(QWidget *page);
    //移除一个页
    void removeNormalIndex(int index);
    //移除当前页
    void removeCurrentPage();
    //清除所有页（不包含fixed页）
    void clearNormalPage();
    //清除所有页（包含fixed页）
    void clearAllPage();
    //添加一个不能关闭且不能拖出的页
    //appendNormalPage+setFixedPage
    void appendFixedPage(QWidget *page);
    //设置某页不能关闭且不能拖出
    void setFixedPage(QWidget *page);
    //设置某页不能关闭且不能拖出
    void setFixedIndex(int index);

    //检查index是否有效
    bool indexValid(int index) const;
    //检查widget是否为有效页
    bool pageValid(QWidget *page) const;

protected:
    void showEvent(QShowEvent *event);

private:
    //初始化tab栏
    void initTabBar();
    //弹出一个页面
    void popPage(QWidget *page);

private:
    //保存不能拖出、没有关闭按钮的tab页（如主页）
    QList<QWidget*> fixedPage;
};

#endif // MYTABWIDGET_H
