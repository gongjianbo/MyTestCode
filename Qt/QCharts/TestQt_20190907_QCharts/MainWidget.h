#ifndef MAINWIDGET_H
#define MAINWIDGET_H

#include <QHash>
#include <QList>
#include <QTimer>

#include <QWidget>
#include <QtCharts>
QT_CHARTS_USE_NAMESPACE

QT_BEGIN_NAMESPACE
namespace Ui { class MainWidget; }
QT_END_NAMESPACE

class MainWidget : public QWidget
{
    Q_OBJECT

public:
    struct LineData
    {
        int value;
        QDateTime time;
    };
    MainWidget(QWidget *parent = nullptr);
    ~MainWidget();
    void init();

    void appendLine(const QString &name,const QColor &color);
    void removeLine(const QString &name);
    void addData(const QString &name,int value,QDateTime time);
    void addData(const QString &name,const QList<LineData> &datas);
    void clearData(const QString &name);

private:
    void updateView();
    void addData(QLineSeries * series,double x,double y);

private:
    Ui::MainWidget *ui;

    QChart *chart;
    QValueAxis *yAxis;
    QDateTimeAxis *xAxis;

    //测试发现QSplineSeries在数据量少并且不需要频繁刷新的情况下好用点
    QHash<QString,QLineSeries*> seriesTable;
    QTimer *dataTimer;
    //QTimer *movingTimer;//从右往左移
};
#endif // MAINWIDGET_H
