#include <QApplication>
#include <QWidget>
#include <QPushButton>
#include <QMouseEvent>
#include <QDebug>

int counter = 0;

class MyButton : public QPushButton
{
    Q_OBJECT
public:
    using QPushButton::QPushButton;

    bool event(QEvent *evt) override{
        if(evt->type()==QEvent::MouseButtonPress){
            //4
            qDebug()<<++counter<<__FUNCTION__<<evt;
        }
        return QPushButton::event(evt);
    }

protected:
    void mousePressEvent(QMouseEvent *evt) override{
        //5
        qDebug()<<++counter<<__FUNCTION__<<evt;
        QPushButton::mousePressEvent(evt);
    }
};

class MyWidget : public QWidget
{
    Q_OBJECT
public:
    MyWidget(QWidget *parent = nullptr)
        : QWidget(parent)
        , button(new MyButton("press", this)){
        button->installEventFilter(this);
    }

    bool eventFilter(QObject *watched, QEvent *evt) override{
        if(watched == button && evt->type()==QEvent::MouseButtonPress){
            //3
            qDebug()<<++counter<<__FUNCTION__<<evt;
        }
        return false;
    }

private:
    QPushButton *button;
};

class MyApp : public QApplication
{
    Q_OBJECT
public:
    using QApplication::QApplication;

    bool event(QEvent * evt) override{
        //仅处理qApp相关的事件，本例的点击不会触发
        if(evt->type()==QEvent::MouseButtonPress){
            qDebug()<<++counter<<__FUNCTION__<<evt;
        }
        return QApplication::event(evt);
    }
    bool notify(QObject *obj, QEvent *evt) override{
        if(evt->type()==QEvent::MouseButtonPress){
            //1
            qDebug()<<++counter<<__FUNCTION__<<obj<<evt;
        }
        return QApplication::notify(obj, evt);
    }
};

class MyFilter : public QObject
{
    Q_OBJECT
public:
    using QObject::QObject;

    bool eventFilter(QObject *obj, QEvent *evt) override{
        if(evt->type()==QEvent::MouseButtonPress){
            //2
            qDebug()<<++counter<<__FUNCTION__<<obj<<evt;
        }
        return false;
    }
};

int main(int argc, char *argv[])
{
    MyApp app(argc, argv);
    MyFilter filter;
    app.installEventFilter(&filter);
    MyWidget outer;
    outer.setObjectName("outer");
    outer.resize(300, 200);
    MyWidget inner(&outer);
    inner.setObjectName("inner");
    inner.resize(300, 200);
    outer.show();
    return app.exec();
}

#include "main.moc"
