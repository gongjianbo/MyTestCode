#ifndef QMLPAINTER_H
#define QMLPAINTER_H

#include <QObject>
#include <QPainter>

class QmlPainter : public QObject
{
    Q_OBJECT
public:
    explicit QmlPainter(QPainter *painter=nullptr);
    ~QmlPainter();

public slots:
    void drawLine(int x1, int y1, int x2, int y2);

private:
    QPainter *painter;
};

#endif // QMLPAINTER_H
