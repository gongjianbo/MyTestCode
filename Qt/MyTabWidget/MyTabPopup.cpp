#include "MyTabPopup.h"

#include <QMouseEvent>
#include <QVBoxLayout>
#include <QDebug>

MyTabPopup::MyTabPopup(QWidget *parent)
    : QDialog(parent)
{

}

void MyTabPopup::setContentWidget(QWidget *page)
{
    if(!page)
        return;
    content=page;
    QVBoxLayout *layout=new QVBoxLayout(this);
    layout->setMargin(0);
    layout->addWidget(page);
}

QWidget* MyTabPopup::getContentWidget()
{
    return content;
}

bool MyTabPopup::event(QEvent *event)
{
    switch(event->type())
    {
    case QEvent::MouseButtonRelease:
    case QEvent::NonClientAreaMouseButtonRelease:
    {
        QMouseEvent *e=static_cast<QMouseEvent*>(event);
        if(e&&e->button()==Qt::LeftButton)
        {
            emit dragRelease(e->globalPos());
        }
    }
        break;
    }
    return QDialog::event(event);
}

