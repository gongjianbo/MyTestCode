#pragma once
#include <QEvent>
#include <QString>

class CustomEvent : public QEvent
{
public:
    // 提供唯一的 id 作为父类构造的参数
    // QEvent::registerEventType(id) 可用来产生唯一 id，参数 id 已用会返回未使用的 id
    enum CustomType {
        IsCustom = User + 1
    };
    explicit CustomEvent(const QString &msg);
    ~CustomEvent();

    const QString &message() const;

private:
    // 事件传递的内容，自定义
    QString mMessage;
};
