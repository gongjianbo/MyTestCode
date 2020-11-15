#include "FramelessHelper.h"

#include <QCursor>
#include <QEnterEvent>
#include <QMouseEvent>
#include <QDebug>

FramelessHelper::FramelessHelper(QObject *parent)
    : QObject(parent)
{

}

int FramelessHelper::getBorderWidth() const
{
    return borderWidth;
}

void FramelessHelper::setBorderWidth(int width)
{
    if(borderWidth!=width){
        borderWidth=width;
        emit borderWidthChanged();
    }
}

bool FramelessHelper::getMoveEnable() const
{
    return moveEnable;
}

void FramelessHelper::setMoveEnable(bool enable)
{
    if(moveEnable!=enable){
        moveEnable=enable;
        emit moveEnableChanged();
    }
}

bool FramelessHelper::getMoving() const
{
    return moveEnable&&moving;
}

void FramelessHelper::setMoving(bool state)
{
    if(moving!=state){
        moving=moveEnable&&state;
        emit movingChanged();
    }
}

bool FramelessHelper::getResizeEnable() const
{
    return resizeEnable;
}

void FramelessHelper::setResizeEnable(bool enable)
{
    if(resizeEnable!=enable){
        resizeEnable=enable;
        emit resizeEnableChanged();
    }
}

bool FramelessHelper::getResizing() const
{
    return resizeEnable&&resizing;
}

void FramelessHelper::setResizing(bool state)
{
    if(resizing!=state){
        resizing=resizeEnable&&state;
        emit resizingChanged();
    }
}

void FramelessHelper::setWindow(QQuickWindow *newWindow)
{
    if(newWindow&&newWindow!=window){
        if(window)
            window->removeEventFilter(this);
        window=newWindow;
        window->installEventFilter(this);
        emit windowChanged();
    }
}

void FramelessHelper::setTitle(QQuickItem *newTitle)
{
    if(newTitle&&newTitle!=title){
        if(title)
            title->removeEventFilter(this);
        title=newTitle;
        title->installEventFilter(this);
        emit titleChanged();
    }
}

bool FramelessHelper::eventFilter(QObject *watched, QEvent *event)
{
    //qDebug()<<watched<<event;
    if(watched==window){
        filterWindowEvent(event);
    }else if(watched==title){
        filterTitleEvent(event);
    }
    return false;
}

void FramelessHelper::filterWindowEvent(QEvent *event)
{
    if(!window)
        return;
    switch (event->type()) {
    //case QEvent::CursorChange: break;
    case QEvent::Enter:
        //根据位置更新鼠标样式
        updatePosition(static_cast<QEnterEvent*>(event)->pos());
        break;
    case QEvent::Leave:
        //恢复鼠标样式
        resetCuror();
        break;
    case QEvent::CursorChange:
        //跑到别的区域上了
    case QEvent::UpdateRequest:
        //QExposeEvent的时候会设置为默认样式，这里重置回来
        if(getResizing()||getMoving())
            updateCursor(cursorArea);
        break;
    case QEvent::MouseButtonPress:
        //screenPosTemp = static_cast<QMouseEvent*>(event)->screenPos().toPoint();
        screenPosTemp=QCursor::pos();
        geometryTemp = window->geometry();
        //非边框区域FContentArea
        if (getResizeEnable()&&cursorArea!=FContentArea&&!windowIsMaxed()) {
            //非最大化时点击了边框，且允许缩放
            setResizing(true);
        }
        break;
    case QEvent::MouseButtonRelease:
        geometryTemp = window->geometry();
        //非拖动标题栏时释放鼠标
        if(!getMoving()){
            setResizing(false);
            updatePosition(static_cast<QMouseEvent*>(event)->pos());
        }
        break;
    case QEvent::MouseMove:
        mouseMoveEvent(static_cast<QMouseEvent*>(event));
        break;
    default: break;
    }
}

void FramelessHelper::filterTitleEvent(QEvent *event)
{
    if(!window||!title)
        return;
    switch (event->type()) {
    case QEvent::MouseButtonPress:
        //点击标题栏
        if (getMoveEnable()) {
            if (windowIsMaxed()) {
                //最大化状态下点击标题栏
            }else if(cursorArea==FContentArea){
                //非边框区域时可以拖动
                setMoving(true);
            }
        }
        break;
    case QEvent::MouseButtonDblClick:
        //双击标题栏，切换最大和普通大小
        if(windowIsMaxed()){
            window->showNormal();
        }else{
            window->showMaximized();
        }
        break;
    case QEvent::MouseButtonRelease:
        //拖动标题栏时释放鼠标
        if(getMoving()){
            setMoving(false);
            updatePosition(static_cast<QMouseEvent*>(event)->pos());
            QRect geometry=window->geometry();
            //如果拖到了标题栏就最大化
            if(geometry.y()<0){
                geometry.moveTop(0);
                window->setGeometry(geometry);
                window->showMaximized();
            }
        }
        break;
    case QEvent::MouseMove:
        //最大化时拖动标题栏就恢复为普通大小并可拖动
        if (getMoveEnable()&&windowIsMaxed()) {
            QMouseEvent *mouse_event=static_cast<QMouseEvent*>(event);
            const int old_width=window->width();
            const int old_pos=mouse_event->pos().x();
            window->showNormal();
            QRect geometry=window->geometry();
            const QPoint cursor_pos=QCursor::pos();
            //标题栏上，鼠标所在x按照原来的比例设置，y不变
            geometry.moveLeft(cursor_pos.x()-geometry.width()*(old_pos/(double)old_width));
            geometry.moveTop(cursor_pos.y()-title->height()/2);
            window->setGeometry(geometry);
            geometryTemp=geometry;
            setMoving(true);
        }
        break;
    default: break;
    }
}

void FramelessHelper::mouseMoveEvent(QMouseEvent *event)
{
    if(getMoving()){
        //按住标题栏拖动
        event->accept();
        const QPoint move_vec=QCursor::pos()-screenPosTemp;
        window->setGeometry(QRect(geometryTemp.topLeft()+move_vec,geometryTemp.size()));
    }else if(getResizing()){
        //按住边框拖拽大小
        event->accept();
        const QPoint move_vec=QCursor::pos()-screenPosTemp;
        //每个方向单独计算
        //然后判断计算出来的pos和size是否有效,大于最小尺寸
        //因为每个方向固定的边不一样，所以单独处理
        QRect new_geometry=geometryTemp;
        //横项调整
        switch (cursorArea&0xF0) {
        case FLeftArea: //左侧
            new_geometry.setLeft(geometryTemp.left()+move_vec.x());
            if (new_geometry.width()<window->minimumWidth()){
                new_geometry.setLeft(geometryTemp.right()-window->minimumWidth());
            }
            break;
        case FRightArea: //右侧
            new_geometry.setRight(geometryTemp.right()+move_vec.x());
            if (new_geometry.width()<window->minimumWidth()){
                new_geometry.setRight(geometryTemp.left()+window->minimumWidth());
            }
            break;
        default: break;
        }
        //竖向调整
        switch (cursorArea&0x0F) {
        case FTopArea: //顶部
            new_geometry.setTop(geometryTemp.top()+move_vec.y());
            if(new_geometry.height()<window->minimumHeight())
                new_geometry.setTop(geometryTemp.bottom()-window->minimumHeight());
            break;
        case FBottomArea: //底部
            new_geometry.setBottom(geometryTemp.bottom()+move_vec.y());
            if(new_geometry.height()<window->minimumHeight())
                new_geometry.setBottom(geometryTemp.top()+window->minimumHeight());
            break;
        default: break;
        }
        window->setGeometry(new_geometry);
    }else if(getResizeEnable()){
        //根据位置更新鼠标样式
        updatePosition(event->pos());
    }
}

bool FramelessHelper::windowIsMaxed() const
{
    //判断窗口是否最大化
    return (window&&(window->visibility()==QWindow::Maximized
                     ||window->visibility()==QWindow::FullScreen));
}

void FramelessHelper::updatePosition(const QPoint &pos)
{
    if(!window||windowIsMaxed())
        return;
    //根据鼠标坐标判断所在区域
    int pos_area=cursorArea;
    //可拖拽大小时才判断
    if (resizeEnable)
    {
        if (pos.x()<borderWidth) {
            pos_area=0x10;
        }else if (pos.x()>window->width()-borderWidth) {
            pos_area=0x20;
        }else {
            pos_area=0x00;
        }
        if (pos.y()<borderWidth) {
            pos_area+=0x01;
        }else if (pos.y()>window->height()-borderWidth) {
            pos_area+=0x02;
        }else {
            pos_area+=0x00;
        }
    }
    if (pos_area == cursorArea)
        return;
    cursorArea=(FramelessArea)pos_area;
    updateCursor(cursorArea);
}

void FramelessHelper::updateCursor(int area)
{
    //根据鼠标悬停位置更换鼠标形状
    switch (area) {
    case FLeftArea:
    case FRightArea:
        window->setCursor(Qt::SizeHorCursor);
        break;
    case FTopArea:
    case FBottomArea:
        window->setCursor(Qt::SizeVerCursor);
        break;
    case FLeftTopCorner:
    case FRightBottomCorner:
        window->setCursor(Qt::SizeFDiagCursor);
        break;
    case FRightTopCorner:
    case FLeftBottomCorner:
        window->setCursor(Qt::SizeBDiagCursor);
        break;
    default:
        window->setCursor(Qt::ArrowCursor);
        break;
    }
}

void FramelessHelper::resetCuror()
{
    if(!window)
        return;
    //重置为默认鼠标样式
    cursorArea=FContentArea;
    window->setCursor(Qt::ArrowCursor);
}
