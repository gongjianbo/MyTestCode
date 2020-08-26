#ifndef SORTHEADERVIEW_H
#define SORTHEADERVIEW_H

#include <QHeaderView>

/**
 * @brief 自定义表头，以支持取消排序状态
 */
class SortHeaderView : public QHeaderView
{
    Q_OBJECT
public:
    //排序状态
    enum SortOrder
    {
        NoOrder = 0 //未排序
        , DescOrder //降序
        , AscOrder  //升序
    };
public:
    explicit SortHeaderView(Qt::Orientation orientation = Qt::Horizontal,
                            QWidget *parent = nullptr);

    //不排序的行列
    //（为什么放header不放proxy？因为排序是点击header调用的）
    QList<int> getUnsortIndexs() const;
    void setUnsortIndexs(const QList<int> &rowOrColumns);
    void appendUnsortIndex(int rowOrColumn);

protected:
    //显示的时候同步indicator三角状态，
    //因为初始化可能是用的sortByColumn来排序，但是这个接口不是虚函数没法自定义
    //所以就在显示的时候同步下状态
    void showEvent(QShowEvent *event) override;
    // 点击表头
    void mousePressEvent(QMouseEvent *event) override;
    void mouseReleaseEvent(QMouseEvent *event) override;
    // 根据当前排序状态得到下一个状态
    SortOrder getNextOrder(SortOrder order) const;

signals:
    //因为view里关联了自带的sortIndicatorChanged，
    //所以自定义一个，避免冲突
    void sortOrderChanged(int index, SortOrder order);

private:
    // 不进行排序的行列
    QList<int> unsortIndexs;

    //鼠标按下时对应原model index
    int pressLogicIndex = -1;
    //鼠标按下时对应显示的index，可能是交换了位置的
    int pressVisualIndex = -1;
    //上一次点击对应的logic index
    int previousLogicIndex = -1;
    //当前排序状态
    SortOrder sortOrder = NoOrder;
};

#endif // SORTHEADERVIEW_H
