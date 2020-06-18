#include "MyComboBox.h"

#include <QMouseEvent>
#include <QLineEdit>
#include <QStyleOptionViewItem>
#include <QStyle>
#include <QScrollBar>

#include <QDebug>

ComboView::ComboView(QWidget *parent)
    : QListWidget(parent)
{

}

QRect ComboView::visualRect(const QModelIndex &index) const
{
    QRect rect=QListWidget::visualRect(index);
    int width=this->width();
    if(verticalScrollBar()->isVisible()){
        width-=verticalScrollBar()->width();
    }
    rect.setWidth(width);
    return rect;
}

ComboItem::ComboItem(const QString &text, QWidget *parent)
    : QWidget(parent),
      textValue(text),
      btn(new QPushButton(this))
{
    QHBoxLayout *layout=new QHBoxLayout(this);
    layout->addStretch();
    layout->addWidget(btn);
    layout->setMargin(0);
    layout->setSpacing(0);

    connect(btn,&QPushButton::clicked,[this]{
        emit itemClicked(textValue);
    });
}

ComboItem::~ComboItem()
{
    qDebug()<<"~delete"<<textValue;
}

QString ComboItem::text() const
{
    return textValue;
}

ComboDelegate::ComboDelegate(QObject *parent)
    : QStyledItemDelegate(parent)
{

}

void ComboDelegate::paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const
{
    QStyleOptionViewItem opt(option);
    //去掉焦点带来的虚线框
    opt.state &= ~QStyle::State_HasFocus;
    
    initStyleOption(&opt, index);
    QStyledItemDelegate::paint(painter, opt, index);
}

MyComboBox::MyComboBox(QWidget *parent)
    : QComboBox(parent),
      itemList(new ComboView(this))
{
    //itemList->setTextElideMode(Qt::ElideNone);
    setModel(itemList->model());
    setView(itemList);
    setItemDelegate(new ComboDelegate(this));
}

void MyComboBox::setRemovableItems(const QStringList &items)
{
    //combox的additem insertitem不是虚函数
    //实现里时调用的model->insertRow，但是懒得再去重写listmodel-view，就新增一个接口
    itemList->clear();
    if(items.isEmpty())
        return;

    for(int i=0;i<items.count();i++)
    {
        QListWidgetItem* widget_item = new QListWidgetItem(itemList);
        ComboItem *item=new ComboItem(items.at(i),itemList);
        widget_item->setData(Qt::DisplayRole,items.at(i));
        //widget_item->setData(Qt::TextAlignmentRole,int(Qt::AlignRight|Qt::AlignVCenter));
        itemList->setItemWidget(widget_item,item);

        connect(item,&ComboItem::itemClicked,this,[this,item,widget_item](){
            //take移除item后没有刷新弹框大小，干脆隐藏掉先
            hidePopup();

            itemList->takeItem(itemList->row(widget_item));
            delete widget_item;
            emit itemRemoved(item->text());
        });
    }
}

