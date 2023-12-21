#pragma once
#include <qscreen.h>
#include <private/qtquickglobal_p.h>
#include <qquickwindow.h>
#include <qqmlparserstatus.h>
#include "mywindowattached.h"

class MyQuickWindowPrivate;

class MyQuickWindow : public QQuickWindow, public QQmlParserStatus
{
    Q_OBJECT
    Q_INTERFACES(QQmlParserStatus)

    Q_PROPERTY(bool visible READ isVisible WRITE setVisible NOTIFY visibleChanged)
    Q_PROPERTY(Visibility visibility READ visibility WRITE setVisibility NOTIFY visibilityChanged)
    Q_PROPERTY(QObject *screen READ screen WRITE setScreen NOTIFY screenChanged)
    Q_PROPERTY(qreal dpiRatio READ dpiRatio NOTIFY dpiRatioChanged)
    QML_ATTACHED(MyQuickWindowAttached)

public:
    MyQuickWindow(QWindow *parent = nullptr);

    void setVisible(bool visible);
    void setVisibility(Visibility visibility);

    QObject *screen() const;
    void setScreen(QObject *screen);

    qreal dpiRatio();
    void setDpiRatio(qreal ratio);
    static MyQuickWindowAttached *qmlAttachedProperties(QObject *object);

Q_SIGNALS:
    void visibleChanged(bool arg);
    void visibilityChanged(QWindow::Visibility visibility);
    void screenChanged();
    void dpiRatioChanged(qreal ratio);

protected:
    void classBegin() override;
    void componentComplete() override;

private Q_SLOTS:
    void setWindowVisibility();

private:
    bool transientParentVisible();
    void onScreenChange(QScreen *screen);
    void calcDpi();

private:
    Q_DISABLE_COPY(MyQuickWindow)
    Q_DECLARE_PRIVATE(MyQuickWindow)
};

QML_DECLARE_TYPE(MyQuickWindow)
