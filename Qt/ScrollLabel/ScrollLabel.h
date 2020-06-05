#ifndef SCROLLLABEL_H
#define SCROLLLABEL_H

#include <QLabel>
#include <QBasicTimer>

//滚动文字的label
class ScrollLabel : public QLabel
{
    Q_OBJECT
public:
    //滚动方向
    enum ScrollDirection{
        RightToLeft=1,
        LeftToRight=2
    };
public:
    explicit ScrollLabel(QWidget *parent = nullptr);

    //滚动方向
    ScrollLabel::ScrollDirection getDirection() const;
    void setDirection(ScrollLabel::ScrollDirection direction);

    //刷新间隔
    int getInterval() const;
    void setInterval(int interval);

protected:
    //basictimer定时器触发
    void timerEvent(QTimerEvent *event) override;
    //绘制
    void paintEvent(QPaintEvent *event) override;
    //大小变化时重新计算
    void resizeEvent(QResizeEvent *event) override;

private:
    //滚动定时器
    //也可以使用QTimer QTimeLine QAnimation等实现
    QBasicTimer scrollTimer;
    int interval=20;
    //偏移量
    int offset=0;
    int textWidth=0;
    int labelWidth=0;
    //默认右往左
    ScrollDirection direction=RightToLeft;
};

#endif // SCROLLLABEL_H
