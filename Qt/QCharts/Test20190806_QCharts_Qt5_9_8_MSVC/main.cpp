#include <QtWidgets/QApplication>
#include <QtWidgets/QMainWindow>
#include <QtCharts/QChartView>
#include <QtCharts/QBarSeries>
#include <QtCharts/QBarSet>
#include <QtCharts/QLegend>
#include <QtCharts/QBarCategoryAxis>

QT_CHARTS_USE_NAMESPACE

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

//![1]
    QBarSet *set0 = new QBarSet("Jane");
    QBarSet *set1 = new QBarSet("John");
    QBarSet *set2 = new QBarSet("Axel");
    QBarSet *set3 = new QBarSet("Mary");
    QBarSet *set4 = new QBarSet("Samantha");

    *set0 << 1 << 2 << 3 << 4 << 5 << 6;
    *set1 << 5 << 0 << 0 << 4 << 0 << 7;
    *set2 << 3 << 5 << 8 << 13 << 8 << 5;
    *set3 << 5 << 6 << 7 << 3 << 4 << 5;
    *set4 << 9 << 7 << 5 << 3 << 1 << 2;
//![1]

//![2]
    QBarSeries *series = new QBarSeries();
    series->append(set0);
    series->append(set1);
    series->append(set2);
    series->append(set3);
    series->append(set4);

//![2]

//![3]
    QChart *chart = new QChart();
    chart->addSeries(series);
    chart->setTheme(QChart::ChartThemeQt);
    chart->setTitle("Simple barchart example");
    chart->setAnimationOptions(QChart::SeriesAnimations);
    chart->setBackgroundVisible(false);
    //chart->setBackgroundBrush(QBrush("darkCyan"));
//![3]

//![4]
    QStringList categories;
    categories << "Jan" << "Feb" << "Mar" << "Apr" << "May" << "Jun";
    QBarCategoryAxis *axis = new QBarCategoryAxis();
    axis->append(categories);
    chart->createDefaultAxes();
    chart->setAxisX(axis, series);
//![4]

//![5]
    chart->legend()->setVisible(true);
    chart->legend()->setAlignment(Qt::AlignBottom);
//![5]

//![6]
    QChartView *chartView = new QChartView(chart);
    chartView->setRenderHint(QPainter::Antialiasing);
    //chartView->setContentsMargins(0,10,0,10);
    //chartView->setBackgroundBrush(QBrush(Qt::yellow));
    //chartView->setStyleSheet("");
    //chartView->setBackgroundBrush(Qt::transparent);

//![6]

//![7]
    QMainWindow window;
    window.setCentralWidget(chartView);
    window.setContentsMargins(10,10,10,10);
    window.setStyleSheet(R"(QMainWindow{ background-color:green;}
                         QMainWindow QWidget{ background-color:cyan;}
                         QGraphicsView{ background-image:url(:/backimg.png);})");
    window.resize(420, 300);
    window.show();
//![7]

    return a.exec();
}

