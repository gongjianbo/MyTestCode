#ifndef MAINWIDGET_H
#define MAINWIDGET_H

#include <QWidget>
#include <QPaintEvent>
#include <QResizeEvent>
#include <QMouseEvent>
#include <QHoverEvent>
#include <QPainter>
#include <QList>
#include <QPropertyAnimation>

struct PieSlice
{
    explicit PieSlice(const QString &name,double value=0.0)
        :itemName(name),itemValue(value){
    }
    QString itemName;   //项名
    double itemValue;   //项值
    double percentage;  //item值占总值的百分比
    double startAngle;  //起始角度[0,360)
    double angleSpan;   //角度值[0,360)
    QColor color;       //颜色
};

namespace Ui {
class MainWidget;
}

class MainWidget : public QWidget
{
    Q_OBJECT
    Q_PROPERTY(double pieProgress READ getPieProgress WRITE setPieProgress)
public:
    explicit MainWidget(QWidget *parent = nullptr);
    ~MainWidget();

    //初始化
    void init();

    //相关操作
    void appendSlice(const PieSlice &slice); //添加数据
    void clearSlice(); //清除容器数据
    void mouseMove(const QPoint &pos);  //hover移动
    void mouseLeave(); //hover离开

    //用于属性动画
    double getPieProgress() const;
    void setPieProgress(double pro); //范围[0,1]

protected:
    bool event(QEvent *event) override;
    void paintEvent(QPaintEvent *event) override;
    void resizeEvent(QResizeEvent *event) override;

private:
    Ui::MainWidget *ui;
    QPoint mousePos;      //记录鼠标hover轨迹
    bool hoveredFlag;     //是否hover
    int hoveredIndex;     //当前选择的index

    int pieMargin;        //边距
    QRectF pieRect;       //范围
    QPointF pieCenterPos; //中心
    int pieRadius;        //半径
    double pieValueCount; //总的值

    double animationProgress;      //总的进度，对应qt属性值
    int animationIndex;            //动画中的index
    double animationIndexProgress; //动画中index的进度
    QPropertyAnimation animation;

    QList<PieSlice> pieSliceList;  //slice数据容器
     //注意：上面的index是list容器中的index值
};

#endif // MAINWIDGET_H
