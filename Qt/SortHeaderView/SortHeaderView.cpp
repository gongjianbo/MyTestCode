#include "SortHeaderView.h"

#include <QMouseEvent>
#include <QCursor>

SortHeaderView::SortHeaderView(Qt::Orientation orientation, QWidget *parent)
    : QHeaderView(orientation,parent)
{
    //禁用默认的点击处理，这样就不会触发默认排序
    //自己处理点击来设置排序
    setSectionsClickable(false);
}

QList<int> SortHeaderView::getUnsortIndexs() const
{
    return unsortIndexs;
}

void SortHeaderView::setUnsortIndexs(const QList<int> &rowOrColumns)
{
    unsortIndexs = rowOrColumns;
}

void SortHeaderView::appendUnsortIndex(int rowOrColumn)
{
    unsortIndexs.push_back(rowOrColumn);
}

void SortHeaderView::showEvent(QShowEvent *event)
{
    QHeaderView::showEvent(event);
    //因为初始化可能是用的sortByColumn来排序，但是这个接口不是虚函数没法自定义
    //所以就在显示的时候同步下状态
    previousLogicIndex = sortIndicatorSection();
    if (previousLogicIndex >= 0) {
        sortOrder = (sortIndicatorOrder() == Qt::AscendingOrder) ? AscOrder : DescOrder;
    }
    else {
        sortOrder = NoOrder;
    }
}

void SortHeaderView::mousePressEvent(QMouseEvent *event)
{
    QHeaderView::mousePressEvent(event); //注意顺序

    if (event->button() != Qt::LeftButton)
        return;
    const int pos = (orientation() == Qt::Horizontal) ? event->x() : event->y();
    const int index = logicalIndexAt(pos);
    Qt::CursorShape shape = cursor().shape();

    //先判断index是否为有效的section，
    //判断cursor-shape是为了过滤点resize-handle情况
    pressLogicIndex = -1;
    pressVisualIndex = -1;
    if (index >= 0 && shape != Qt::SplitHCursor&&shape != Qt::SplitVCursor)
    {
        //排除不排序的index
        if (!unsortIndexs.contains(index))
        {
            pressLogicIndex = index;
            pressVisualIndex = visualIndex(index);
        }
    }
}

void SortHeaderView::mouseReleaseEvent(QMouseEvent *event)
{
    QHeaderView::mouseReleaseEvent(event); //注意顺序

    if (event->button() != Qt::LeftButton)
        return;
    const int pos = (orientation() == Qt::Horizontal) ? event->x() : event->y();
    const int index = logicalIndexAt(pos);
    const int visual_index = visualIndex(index);

    //弹起后再进行排序操作
    //此处还可以判断下鼠标的移动距离，暂略
    if (pressLogicIndex >= 0 && visual_index == pressVisualIndex)
    {
        //和上次点击的index比较，相同的就轮换排序状态，不同的就降序
        //放release不放pressed是为了避免双击时判断不准
        if (pressLogicIndex == previousLogicIndex)
        {
            sortOrder = getNextOrder(sortOrder);
        }
        else
        {
            sortOrder = getNextOrder(NoOrder);
        }
        emit sortOrderChanged(pressLogicIndex, sortOrder);
        previousLogicIndex = pressLogicIndex;
    }
    //注释不复位是为了避免双击时判断不准
    //pressLogicIndex=-1;
}

SortHeaderView::SortOrder SortHeaderView::getNextOrder(SortHeaderView::SortOrder order) const
{
    //状态循环：无排序-降序-升序
    switch (order)
    {
    case NoOrder: return DescOrder;
    case DescOrder: return AscOrder;
    case AscOrder: break;
    }
    return NoOrder;
}
