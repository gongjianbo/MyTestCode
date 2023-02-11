#include "FpsItem.h"
#include <QQuickWindow>
#include <QTimer>

FpsItem::FpsItem(QQuickItem *parent)
    : QQuickItem(parent)
{
    QTimer *timer = new QTimer(this);
    connect(timer, &QTimer::timeout, this, [this]{
        setFps(mFrameCounter);
        mFrameCounter = 0;
    });
    connect(this, &QQuickItem::windowChanged, this, [this]{
        if (window()) {
            connect(window(), &QQuickWindow::afterRendering,
                    this, [this]{
                mFrameCounter++;
            }, Qt::DirectConnection);
        }
    });
    timer->start(1000);
}

int FpsItem::getFps() const
{
    return mFps;
}

void FpsItem::setFps(int fps)
{
    if (mFps != fps) {
        mFps = fps;
        emit fpsChanged();
    }
}
