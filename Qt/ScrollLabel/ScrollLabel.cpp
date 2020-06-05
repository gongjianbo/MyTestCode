#include "ScrollLabel.h"

#include <QTimerEvent>
#include <QPaintEvent>
#include <QResizeEvent>
#include <QPainter>

#include <QDebug>

ScrollLabel::ScrollLabel(QWidget *parent)
    : QLabel(parent)
{
    //启动定时器，触发this的timerevent
    scrollTimer.start(interval,this);
}

ScrollLabel::ScrollDirection ScrollLabel::getDirection() const
{
    return direction;
}

void ScrollLabel::setDirection(ScrollLabel::ScrollDirection direction)
{
    if(this->direction!=direction){
        this->direction=direction;
        offset=0;
    }
}

int ScrollLabel::getInterval() const
{
    return interval;
}

void ScrollLabel::setInterval(int interval)
{
    if(this->interval!=interval){
        this->interval=interval;
        scrollTimer.start(interval,this);
    }
}

void ScrollLabel::timerEvent(QTimerEvent *event)
{
    //定时器timeout
    if(event->timerId()==scrollTimer.timerId()){
        event->accept();
        ++offset;
        if(offset>textWidth+labelWidth){
            offset=0;
        }
        update();
    }else{
        QLabel::timerEvent(event);
    }
}

void ScrollLabel::paintEvent(QPaintEvent *event)
{
    event->accept();

    QPainter painter(this);
    const int text_width = painter.fontMetrics().width(text());
    //字体绘制坐标为左下角，y值就是 labelheight-(labelheight-textheight)/2
    //因为取的字体高度还受基线影响，height=descent+ascent，这里去掉descent
    //也可以用Qt5.8提供的capHeight
    /*const int text_height = painter.fontMetrics().capHeight();
    const int text_y = (height()+text_height) / 2;*/
    const int text_height = painter.fontMetrics().height();
    const int text_y = (height()+text_height) / 2-painter.fontMetrics().descent();
    if (textWidth != text_width && text_width > 0) {
        textWidth = text_width;
        offset = 0;
    }else {
        if(direction==RightToLeft){
            //从右往左
            painter.drawText(labelWidth - offset, text_y, text());
        }else{
            //从左往右
            painter.drawText(offset - textWidth, text_y, text());
        }
    }
}

void ScrollLabel::resizeEvent(QResizeEvent *event)
{
    const int old_width = event->oldSize().width();
    const int new_width = event->size().width();
    if (new_width > 10) {
        labelWidth = new_width;
        //新宽度更小，就重置偏移
        if (new_width < old_width) {
            offset = 0;
        }
    }
    QLabel::resizeEvent(event);
}
