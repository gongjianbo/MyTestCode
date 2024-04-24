#pragma once
#include <QCoreApplication>
#include <QHoverEvent>
// quick-private
#include <QtQuick/private/qquickmousearea_p.h>

/**
 * @brief 继承QQuickMouseArea并转发需要的事件
 * @author 龚建波
 * @date 2024-04-24
 */
class MyMouseArea : public QQuickMouseArea
{
    Q_OBJECT
    Q_PROPERTY(QQuickMouseArea *forwardTo READ getForwardTo WRITE setForwardTo NOTIFY forwardToChanged)
public:
    using QQuickMouseArea::QQuickMouseArea;

    QQuickMouseArea *getForwardTo() const {
        return forwardTo;
    }
    void setForwardTo(QQuickMouseArea *forward) {
        if (forwardTo != forward) {
            forwardTo = forward;
            emit forwardToChanged();
        }
    }

protected:
    void hoverEnterEvent(QHoverEvent *event) override {
        QQuickMouseArea::hoverEnterEvent(event);
        doForward(event);
    }
    void hoverMoveEvent(QHoverEvent *event) override {
        QQuickMouseArea::hoverMoveEvent(event);
        doForward(event);
    }
    void hoverLeaveEvent(QHoverEvent *event) override {
        QQuickMouseArea::hoverLeaveEvent(event);
        doForward(event);
    }
    void doForward(QHoverEvent *event) {
        if (!forwardTo) {
            return;
        }
        // TODO 这里没有处理交叠部分，而是直接转发了
        QHoverEvent hover{event->type(),
                          mapToItem(forwardTo, event->posF()),
                          mapToItem(forwardTo, event->oldPosF()),
                          event->modifiers()};
        // 使用send多个MouseArea之间移动时保持一定的处理顺序
        QCoreApplication::sendEvent(forwardTo, &hover);
        // QCoreApplication::sendEvent(forwardTo, event);
    }

signals:
    void forwardToChanged();

private:
    // 事件转发到对应的MouseArea
    QQuickMouseArea *forwardTo{nullptr};
};
