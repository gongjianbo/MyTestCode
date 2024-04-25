#pragma once
#include <QCoreApplication>
#include <QHoverEvent>
#include <QQuickItem>
#include <QDebug>

class MyMouseFilter : public QObject
{
    Q_OBJECT
    Q_PROPERTY(QQuickItem *attach READ getAttach WRITE setAttach NOTIFY attachChanged)
    Q_PROPERTY(QQuickItem *forwardTo READ getForwardTo WRITE setForwardTo NOTIFY forwardToChanged)
public:
    using QObject::QObject;

    QQuickItem *getAttach() const {
        return attach;
    }
    void setAttach(QQuickItem *item) {
        if (item == attach)
            return;
        if (attach) {
            attach->removeEventFilter(this);
        }
        attach = item;
        if (attach) {
            attach->installEventFilter(this);
        }
        emit attachChanged();
    }

    QQuickItem *getForwardTo() const {
        return forwardTo;
    }
    void setForwardTo(QQuickItem *item) {
        if (item == forwardTo)
            return;
        forwardTo = item;
        emit forwardToChanged();
    }

    bool eventFilter(QObject *watched, QEvent *event) override {
        if (!attach || !forwardTo || watched != attach)
            return false;
        switch (event->type()) {
        case QEvent::HoverEnter:
        case QEvent::HoverLeave:
        case QEvent::HoverMove: {
            QHoverEvent *hover_in = static_cast<QHoverEvent *>(event);
            if (hover_in) {
                // TODO 这里没有处理交叠部分，而是直接转发了
                QHoverEvent hover{hover_in->type(),
                                  attach->mapToItem(forwardTo, hover_in->posF()),
                                  attach->mapToItem(forwardTo, hover_in->oldPosF()),
                                  hover_in->modifiers()};
                // 使用send多个MouseArea之间移动时保持一定的处理顺序
                QCoreApplication::sendEvent(forwardTo, &hover);
            }
        } break;
        }
        return false;
    }

signals:
    void attachChanged();
    void forwardToChanged();

private:
    QQuickItem *attach{nullptr};
    QQuickItem *forwardTo{nullptr};
};
