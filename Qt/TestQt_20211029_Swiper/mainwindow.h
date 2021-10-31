#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QPainterPath>
#include <QMouseEvent>
#include <QHoverEvent>
#include <QPainter>
#include <QImage>
#include <QPropertyAnimation>
#include <QTimer>

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT
    Q_PROPERTY(double curIndex READ getCurIndex WRITE setCurIndex NOTIFY curIndexChanged)
public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

    double getCurIndex() const;
    void setCurIndex(double index);

protected:
    bool event(QEvent *e) override;
    void paintEvent(QPaintEvent *event) override;
    void resizeEvent(QResizeEvent *event) override;

private:
    //计算图片位置
    void calcImagePos();
    //计算按钮位置
    void calcBtnPath();
    //切换到下一个图
    void toPrev();
    //切换到上一个图
    void toNext();

signals:
    void curIndexChanged();

private:
    Ui::MainWindow *ui;

    struct ImageNode{
        QImage img;
        double xf{0};
        double yf{0};
        double zf{0};
    };

    //当前图片列表
    QVector<ImageNode> imageList;
    //按钮位置
    QVector<QRectF> btnList;
    //图片z堆叠顺序
    QVector<int> drawList;
    //当前设置的index
    int setIndex{0};
    //属性动画绘制用到的index
    double curIndex{0.0};
    //旋转动画
    QPropertyAnimation animation;
    //自动切换定时器
    QTimer swipTimer;
};
#endif // MAINWINDOW_H
