#ifndef KEYBOARDUTIL_H
#define KEYBOARDUTIL_H

#include <QObject>
#include <QKeyEvent>

class KeyboardUtil : public QObject
{
    Q_OBJECT
    //表示大小写状态的属性
    Q_PROPERTY(bool capslock READ isCapslock NOTIFY capslockChanged)
public:
    explicit KeyboardUtil(QObject *parent = nullptr);

    //请求检测当前大小写
    Q_INVOKABLE void checkCapslock();

    //大小写锁定状态
    bool isCapslock() const;

    //关联watch
    //用于在目标上注册事件过滤器（如attach弹出的对话框）
    Q_INVOKABLE void attachWatch(QObject *watch);

protected:
    bool eventFilter(QObject *watched, QEvent *event) override;

signals:
    void capslockChanged();
};

#endif // KEYBOARDUTIL_H
