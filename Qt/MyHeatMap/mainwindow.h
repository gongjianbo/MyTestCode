#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>

#include <QPainter>
#include <QPaintEvent>
#include <QMouseEvent>

#include <QLinearGradient>
#include <QRadialGradient>

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

//参照JS版：https://blog.csdn.net/HiddlestonCloud/article/details/83743449
//参照Qt版：https://blog.csdn.net/pbe_sedm/article/details/8982357
class MainWindow : public QMainWindow
{
    Q_OBJECT
    struct Point
    {
        int posX;
        int posY;
        int radius;
        int count;
    };
public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

protected:
    void paintEvent(QPaintEvent *event) override;
    void mousePressEvent(QMouseEvent *event) override;

    //添加点，返回该点权重，如果是一次性生成的点可以单独优化
    int appendPoint(const Point &pt);
    //最大权重改变之后重新绘制dataimg
    void drawDataImg();
    //叠加绘制dataimg
    void drawDataPoint(const Point &pt);
    //根据dataimg绘制热力图
    void drawHeatImg();

    //清空
    void clear();

private:
    Ui::MainWindow *ui;

    //整体的透明度[0-255]
    static constexpr int HeatAlpha=200;
    //固定宽高的演示
    static constexpr int ImgWidth=1000;
    static constexpr int ImgHeight=700;
    //最大权重
    int _maxCount=1;
    //权重统计表（把权重单独拿出来，就可以不用遍历数据点来计算了）
    //加()初始化为0
    int *_countTable=new int[ImgWidth*ImgHeight]();

    //数据点
    QList<Point> _posList;
    //绘制透明度
    QImage _dataImg;
    //最终热力图
    QImage _heatImg;
    //颜色表，透明度为0
    QRgb _colorList[256];
};
#endif // MAINWINDOW_H
