#include "mywindowattached.h"
#include "mywindow.h"
#include <qquickwindow.h>
#include <qquickitem.h>

MyQuickWindowAttached::MyQuickWindowAttached(QObject* attachee)
    : QObject(attachee)
    , m_window(nullptr)
{
    m_attachee = qobject_cast<QQuickItem*>(attachee);
    if (m_attachee && m_attachee->window()) // It might not be in a window yet
        windowChange(m_attachee->window());
    if (m_attachee)
        connect(m_attachee, &QQuickItem::windowChanged, this, &MyQuickWindowAttached::windowChange);
}

QWindow::Visibility MyQuickWindowAttached::visibility() const
{
    return (m_window ? m_window->visibility() : QWindow::Hidden);
}

bool MyQuickWindowAttached::isActive() const
{
    return (m_window ? m_window->isActive() : false);
}

QQuickItem *MyQuickWindowAttached::activeFocusItem() const
{
    return (m_window ? m_window->activeFocusItem() : nullptr);
}

QQuickItem *MyQuickWindowAttached::contentItem() const
{
    return (m_window ? m_window->contentItem() : nullptr);
}

int MyQuickWindowAttached::width() const
{
    return (m_window ? m_window->width() : 0);
}

int MyQuickWindowAttached::height() const
{
    return (m_window ? m_window->height() : 0);
}

qreal MyQuickWindowAttached::dpiRatio() const
{
    return (m_window ? m_window->dpiRatio() : 1.0);
}

MyQuickWindow *MyQuickWindowAttached::window() const
{
    return m_window;
}

void MyQuickWindowAttached::windowChange(QQuickWindow *win)
{
    MyQuickWindow *window = qobject_cast<MyQuickWindow*>(win);
    if (window != m_window) {
        MyQuickWindow* oldWindow = m_window;
        m_window = window;

        if (oldWindow)
            oldWindow->disconnect(this);

        emit windowChanged();

        if (!oldWindow || !window || window->visibility() != oldWindow->visibility())
            emit visibilityChanged();
        if (!oldWindow || !window || window->isActive() != oldWindow->isActive())
            emit activeChanged();
        if (!oldWindow || !window || window->activeFocusItem() != oldWindow->activeFocusItem())
            emit activeFocusItemChanged();
        emit contentItemChanged();
        if (!oldWindow || !window || window->width() != oldWindow->width())
            emit widthChanged();
        if (!oldWindow || !window || window->height() != oldWindow->height())
            emit heightChanged();

        if (!window)
            return;

        connect(window, &QQuickWindow::visibilityChanged,
                this, &MyQuickWindowAttached::visibilityChanged);
        connect(window, &QQuickWindow::activeChanged,
                this, &MyQuickWindowAttached::activeChanged);
        connect(window, &QQuickWindow::activeFocusItemChanged,
                this, &MyQuickWindowAttached::activeFocusItemChanged);
        connect(window, &QQuickWindow::widthChanged,
                this, &MyQuickWindowAttached::widthChanged);
        connect(window, &QQuickWindow::heightChanged,
                this, &MyQuickWindowAttached::heightChanged);
        connect(window, &MyQuickWindow::dpiRatioChanged,
                this, &MyQuickWindowAttached::dpiRatioChanged);
    }
}
