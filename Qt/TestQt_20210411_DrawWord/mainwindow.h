#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QPainter>
#include <QPainterPath>
#include <QPainterPathStroker>

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

    void paintEvent(QPaintEvent *event) override;
    void draw(QPainter *painter);
    void drawSeries(QPainter *painter, const QVector<QPointF> &points);

    void mousePressEvent(QMouseEvent *event) override;
    void mouseReleaseEvent(QMouseEvent *event) override;
    void mouseMoveEvent(QMouseEvent *event) override;

    double calcDegree(const QPointF &p1, const QPointF &p2);
    double distanceToWidth(double distance);

private:
    Ui::MainWindow *ui;

    QList<QVector<QPointF>> pointTable;
    QVector<QPointF> pointTemp;
    int maxWidth=30;
};
#endif // MAINWINDOW_H
