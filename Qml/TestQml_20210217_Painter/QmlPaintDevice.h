#ifndef QMLPAINTDEVICE_H
#define QMLPAINTDEVICE_H

#include <QObject>
#include <QQuickPaintedItem>

#include "QmlPainter.h"

class QmlPaintDevice : public QQuickPaintedItem
{
    Q_OBJECT
public:
    explicit QmlPaintDevice(QQuickItem *parent=nullptr);

    void paint(QPainter *painter) override;

signals:
    void doPaint(QmlPainter *painter);
};

#endif // QMLPAINTDEVICE_H
