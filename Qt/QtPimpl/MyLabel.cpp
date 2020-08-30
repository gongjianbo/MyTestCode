#include "MyLabel.h"

class MyLabelPrivate
{
    //本来Qt的private实现可以通过构造函数来传递，
    //但是QWidgetPrivate是私有模块的没法用
    //所以起的同名变量，以使用相关宏
    MyLabel *q_ptr;
    Q_DECLARE_PUBLIC(MyLabel)
public:
    MyLabelPrivate(MyLabel *ptr):q_ptr(ptr){}
    ~MyLabelPrivate()=default;

    void _to_prev()
    {
        Q_Q(MyLabel);
        //往前最小为0
        if(index>0&&index<texts.count())
        {
            index--;
            q->setText(texts.at(index));
        }
    }
    void _to_next()
    {
        Q_Q(MyLabel);
        //往后最大为count-1
        if(index>=00&&index<texts.count()-1)
        {
            index++;
            q->setText(texts.at(index));
        }
    }
    void _add_text(const QString &text)
    {
        Q_Q(MyLabel);
        //设置新的文本值
        texts.push_back(text);
        index=texts.count()-1;
        q->setText(texts.at(index));
    }

    int index=-1;
    QList<QString> texts;
};

MyLabel::MyLabel(QWidget *parent)
    : QLabel(parent)
    ,d_ptr(new MyLabelPrivate(this))
{

}

void MyLabel::toPrev()
{
    Q_D(MyLabel);
    d->_to_prev();
}

void MyLabel::toNext()
{
    Q_D(MyLabel);
    d->_to_next();
}

void MyLabel::addText(const QString &text)
{
    Q_D(MyLabel);
    d->_add_text(text);
}

//#include "moc_mylabel.cpp"
