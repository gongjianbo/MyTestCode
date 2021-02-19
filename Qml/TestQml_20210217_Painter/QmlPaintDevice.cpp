#include "QmlPaintDevice.h"

#include <QQuickWindow>
#include <QThread>
#include <QDebug>

QmlPaintDevice::QmlPaintDevice(QQuickItem *parent)
    : QQuickPaintedItem(parent)
{
    qDebug()<<"main"<<QThread::currentThread();

    connect(this, &QQuickItem::windowChanged,
            this, [this](QQuickWindow *window){
        if (window){
            connect(window,&QQuickWindow::beforeRendering,this,[](){
                //qDebug()<<"before"<<QThread::currentThread();
            });
            connect(window,&QQuickWindow::afterRendering,this,[](){
                //qDebug()<<"after"<<QThread::currentThread();
            });
        }
    });

}

void QmlPaintDevice::paint(QPainter *painter)
{
    painter->setRenderHint(QPainter::Antialiasing,true);
    //qDebug()<<"draw"<<QThread::currentThread(); //QSGRenderThread(0x16a9220)
    QmlPainter qml_painter(painter);
    QMetaObject::invokeMethod(this,"doPaint",Qt::DirectConnection,
                              Q_ARG(QVariant,QVariant::fromValue(&qml_painter)));
    //emit doPaint(&qml_painter);
    //QPainter pt(painter->device());
    //pt.drawLine(0,0,100,50);
}
