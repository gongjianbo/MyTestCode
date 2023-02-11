#pragma once
#include <QQuickItem>

class FpsItem : public QQuickItem
{
    Q_OBJECT
    Q_PROPERTY(int fps READ getFps NOTIFY fpsChanged)
public:
    FpsItem(QQuickItem *parent = nullptr);

    int getFps() const;
    void setFps(int fps);

signals:
    void fpsChanged();

private:
    int mFps{0};
    int mFrameCounter{0};
};
