#ifndef FRAMELESSHELPER_H
#define FRAMELESSHELPER_H

#include <QQuickItem>
#include <QQuickWindow>
#include <QEvent>

/**
 * @brief 一个简易的无边框辅助类
 * @author 龚建波
 * @date 2020-11-15
 * @details
 * 之前看有网友用的Window+本地事件来做的
 * Qt5 QML 中的 Window类型
 * 可能是QQuickWindow或者QQuickWindowQmlImpl（子类），后者未导出
 * 所以思路由重写事件改为过滤，但最终的实现效果感觉不大好，resize得时候一样会有闪烁
 * 感觉解决拉伸时闪烁问题还是用橡皮筋好一点
 *
 * 使用说明：
 * 1.注册为QML类型
 * 2.锚定window（Window）和title（MouseArea）两个区域，且设置borderWidth边距
 * FramelessHelper {
 *      window: root
 *      title: title_area
 *      borderWidth: 6
 * }
 * 3.Window设置flags为无边框
 * flags: Qt.Window|Qt.FramelessWindowHint|Qt.WindowMinMaxButtonsHint
 */
class FramelessHelper : public QObject
{
    Q_OBJECT
    //边距
    Q_PROPERTY(int borderWidth READ getBorderWidth WRITE setBorderWidth NOTIFY borderWidthChanged)
    //拖动使能
    Q_PROPERTY(bool moveEnable READ getMoveEnable WRITE setMoveEnable NOTIFY moveEnableChanged)
    //正在拖动
    Q_PROPERTY(bool moving READ getMoving NOTIFY movingChanged)
    //拉伸缩放使能
    Q_PROPERTY(bool resizeEnable READ getResizeEnable WRITE setResizeEnable NOTIFY resizeEnableChanged)
    //正在拖边框改变大小
    Q_PROPERTY(bool resizing READ getResizing NOTIFY resizingChanged)
    //绑定主窗口，Window类型
    Q_PROPERTY(QQuickWindow* window MEMBER window WRITE setWindow NOTIFY windowChanged)
    //绑定标题栏，MouseArea类型
    Q_PROPERTY(QQuickItem* title MEMBER title WRITE setTitle NOTIFY titleChanged)
private:
    //区域划分-九宫格，便于判断当前点击位置
    //竖向上中下0x01-0x02-0x04
    //横向左中右0x10-0x20-0x40
    //判断时分别取pos-x和y判断区域进行叠加
    enum FramelessArea
    {
        FContentArea = 0x00 //内容区域
        ,FLeftArea = 0x10 //左侧
        ,FRightArea = 0x20 //右侧
        ,FTopArea = 0x01 //顶部
        ,FBottomArea = 0x02 //底部
        ,FLeftTopCorner = 0x11 //左上角
        ,FRightTopCorner = 0x21 //右上角
        ,FLeftBottomCorner = 0x12 //左下角
        ,FRightBottomCorner = 0x22 //右下角
    };
public:
    explicit FramelessHelper(QObject *parent = nullptr);

    int getBorderWidth() const;
    void setBorderWidth(int width);

    bool getMoveEnable() const;
    void setMoveEnable(bool enable);

    bool getMoving() const;
    void setMoving(bool state);

    bool getResizeEnable() const;
    void setResizeEnable(bool enable);

    bool getResizing() const;
    void setResizing(bool state);

    void setWindow(QQuickWindow *newWindow);
    void setTitle(QQuickItem *newTitle);

protected:
    bool eventFilter(QObject *watched, QEvent *event) override;

private:
    //处理窗口相关事件
    void filterWindowEvent(QEvent *event);
    //处理标题栏相关事件
    void filterTitleEvent(QEvent *event);
    //鼠标移动
    void mouseMoveEvent(QMouseEvent *event);
    //判断是否最大化
    bool windowIsMaxed() const;
    //更新鼠标位置
    void updatePosition(const QPoint &pos);
    //判断鼠标位置更新鼠标形状
    void updateCursor(int area);
    //重置为默认鼠标形状
    void resetCuror();

signals:
    void borderWidthChanged();
    void moveEnableChanged();
    void movingChanged();
    void resizeEnableChanged();
    void resizingChanged();
    void windowChanged();
    void titleChanged();

private:
    //边框
    int borderWidth=6;
    //移动标志
    bool moveEnable=true;
    bool moving=false;
    //缩放标志
    bool resizeEnable=true;
    bool resizing=false;
    //暂存鼠标位置信息
    QPoint screenPosTemp=QPoint(0, 0);
    //暂存窗体位置、大小信息
    QRect geometryTemp;
    //当前区域
    FramelessArea cursorArea=FContentArea;
    //窗口-需要绑定Window
    QQuickWindow *window=nullptr;
    //标题栏-需要绑定MouseArea
    QQuickItem *title=nullptr;
};

#endif // FRAMELESSHELPER_H
