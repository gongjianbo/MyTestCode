#pragma once
#include <private/qtquickglobal_p.h>
#include <qqml.h>
#include <QWindow>

class QQuickItem;
class QQuickWindow;
class MyQuickWindow;

class MyQuickWindowAttached : public QObject
{
    Q_OBJECT

    Q_PROPERTY(QWindow::Visibility visibility READ visibility NOTIFY visibilityChanged)
    Q_PROPERTY(bool active READ isActive NOTIFY activeChanged)
    Q_PROPERTY(QQuickItem* activeFocusItem READ activeFocusItem NOTIFY activeFocusItemChanged)
    Q_PROPERTY(QQuickItem* contentItem READ contentItem NOTIFY contentItemChanged)
    Q_PROPERTY(int width READ width NOTIFY widthChanged)
    Q_PROPERTY(int height READ height NOTIFY heightChanged)
    Q_PROPERTY(qreal dpiRatio READ dpiRatio NOTIFY dpiRatioChanged)
    Q_PROPERTY(MyQuickWindow *window READ window NOTIFY windowChanged)

public:
    explicit MyQuickWindowAttached(QObject* attachee);

    QWindow::Visibility visibility() const;
    bool isActive() const;
    QQuickItem* activeFocusItem() const;
    QQuickItem* contentItem() const;
    int width() const;
    int height() const;
    qreal dpiRatio() const;
    MyQuickWindow *window() const;

Q_SIGNALS:
    void visibilityChanged();
    void activeChanged();
    void activeFocusItemChanged();
    void contentItemChanged();
    void widthChanged();
    void heightChanged();
    void dpiRatioChanged();
    void windowChanged();

protected Q_SLOTS:
    void windowChange(QQuickWindow*);

private:
    MyQuickWindow* m_window;
    QQuickItem* m_attachee;
};
