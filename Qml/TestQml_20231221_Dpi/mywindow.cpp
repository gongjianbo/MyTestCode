#include "mywindow.h"
#include "mywindow_p.h"
#include <QDebug>

MyQuickWindow::MyQuickWindow(QWindow *parent)
    : QQuickWindow(*(new MyQuickWindowPrivate), parent)
{
    connect(this, &QWindow::visibleChanged, this, &MyQuickWindow::visibleChanged);
    connect(this, &QWindow::visibilityChanged, this, &MyQuickWindow::visibilityChanged);
    connect(this, &QWindow::screenChanged, this, &MyQuickWindow::onScreenChange);
    calcDpi();
}

void MyQuickWindow::setVisible(bool visible)
{
    Q_D(MyQuickWindow);
    d->visible = visible;
    if (d->complete && (!transientParent() || transientParentVisible()))
        QQuickWindow::setVisible(visible);
}

void MyQuickWindow::setVisibility(Visibility visibility)
{
    Q_D(MyQuickWindow);
    d->visibility = visibility;
    if (d->complete)
        QQuickWindow::setVisibility(visibility);
}

MyQuickWindowAttached *MyQuickWindow::qmlAttachedProperties(QObject *object)
{
    return new MyQuickWindowAttached(object);
}

void MyQuickWindow::classBegin()
{
    Q_D(MyQuickWindow);
    QQmlEngine* e = qmlEngine(this);

    QQmlEngine::setContextForObject(contentItem(), e->rootContext());

    //Give QQuickView behavior when created from QML with QQmlApplicationEngine
    if (QCoreApplication::instance()->property("__qml_using_qqmlapplicationengine") == QVariant(true)) {
        if (e && !e->incubationController())
            e->setIncubationController(incubationController());
    }
    {
        // The content item has CppOwnership policy (set in MyQuickWindow). Ensure the presence of a JS
        // wrapper so that the garbage collector can see the policy.
        QV4::ExecutionEngine *v4 = e->handle();
        QV4::QObjectWrapper::wrap(v4, d->contentItem);
    }
}

void MyQuickWindow::componentComplete()
{
    Q_D(MyQuickWindow);
    d->complete = true;
    QQuickItem *itemParent = qmlobject_cast<QQuickItem *>(QObject::parent());
    const bool transientParentAlreadySet = QQuickWindowPrivate::get(this)->transientParentPropertySet;
    if (!transientParentAlreadySet && itemParent && !itemParent->window()) {
        qDebug() << "window" << title() << "has invisible Item parent" << itemParent << "transientParent"
                 << transientParent() << "declared visibility" << d->visibility << "; delaying show";
        connect(itemParent, &QQuickItem::windowChanged, this,
                &MyQuickWindow::setWindowVisibility, Qt::QueuedConnection);
    } else if (transientParent() && !transientParent()->isVisible()) {
        connect(transientParent(), &QQuickWindow::visibleChanged, this,
                &MyQuickWindow::setWindowVisibility, Qt::QueuedConnection);
    } else {
        setWindowVisibility();
    }
}

void MyQuickWindow::setWindowVisibility()
{
    Q_D(MyQuickWindow);
    if (transientParent() && !transientParentVisible())
        return;

    if (QQuickItem *senderItem = qmlobject_cast<QQuickItem *>(sender())) {
        disconnect(senderItem, &QQuickItem::windowChanged, this, &MyQuickWindow::setWindowVisibility);
    } else if (sender()) {
        disconnect(transientParent(), &QWindow::visibleChanged, this, &MyQuickWindow::setWindowVisibility);
    }

    // We have deferred window creation until we have the full picture of what
    // the user wanted in terms of window state, geometry, visibility, etc.

    if ((d->visibility == Hidden && d->visible) || (d->visibility > AutomaticVisibility && !d->visible)) {
        QQmlData *data = QQmlData::get(this);
        Q_ASSERT(data && data->context);

        QQmlError error;
        error.setObject(this);

        const QQmlContextData* urlContext = data->context;
        while (urlContext && urlContext->url().isEmpty())
            urlContext = urlContext->parent;
        error.setUrl(urlContext ? urlContext->url() : QUrl());

        QString objectId = data->context->findObjectId(this);
        if (!objectId.isEmpty())
            error.setDescription(QCoreApplication::translate("MyQuickWindow",
                                                             "Conflicting properties 'visible' and 'visibility' for Window '%1'").arg(objectId));
        else
            error.setDescription(QCoreApplication::translate("MyQuickWindow",
                                                             "Conflicting properties 'visible' and 'visibility'"));

        QQmlEnginePrivate::get(data->context->engine)->warning(error);
    }

    if (d->visibility == AutomaticVisibility) {
        setWindowState(QGuiApplicationPrivate::platformIntegration()->defaultWindowState(flags()));
        setVisible(d->visible);
    } else {
        setVisibility(d->visibility);
    }
}

QObject *MyQuickWindow::screen() const
{
    return new QQuickScreenInfo(const_cast<MyQuickWindow *>(this), QWindow::screen());
}

void MyQuickWindow::setScreen(QObject *screen)
{
    QQuickScreenInfo *screenWrapper = qobject_cast<QQuickScreenInfo *>(screen);
    QWindow::setScreen(screenWrapper ? screenWrapper->wrappedScreen() : nullptr);
}

qreal MyQuickWindow::dpiRatio()
{
    Q_D(MyQuickWindow);
    return d->dpiRatio;
}

void MyQuickWindow::setDpiRatio(qreal ratio)
{
    Q_D(MyQuickWindow);
    if (qFuzzyCompare(d->dpiRatio, ratio))
        return;
    d->dpiRatio = ratio;
    emit dpiRatioChanged(ratio);
}

bool MyQuickWindow::transientParentVisible()
{
    Q_ASSERT(transientParent());
    if (!transientParent()->isVisible()) {
        // handle case where transient parent is offscreen window
        QWindow *rw = QQuickRenderControl::renderWindowFor(qobject_cast<QQuickWindow*>(transientParent()));
        return rw && rw->isVisible();
    }
    return true;
}

void MyQuickWindow::onScreenChange(QScreen *screen)
{
    Q_D(MyQuickWindow);
    if (d->logicalDpiConnection) {
        disconnect(d->physicalDpiConnection);
        disconnect(d->logicalDpiConnection);
    }
    if (screen) {
        // 切换缩放比时，EnableHighDpiScaling 会触发 physicalDotsPerInchChanged
        // 此时 Qt6 不触发 logicalDotsPerInchChanged
        d->physicalDpiConnection = connect(screen, &QScreen::physicalDotsPerInchChanged, this, &MyQuickWindow::calcDpi);
        d->logicalDpiConnection = connect(screen, &QScreen::logicalDotsPerInchChanged, this, &MyQuickWindow::calcDpi);
        calcDpi();
    }
    emit screenChanged();
}

void MyQuickWindow::calcDpi()
{
    QScreen *cur_screen = QWindow::screen();
    qDebug()<<__FUNCTION__<<cur_screen;
    if (cur_screen) {
        // 逻辑 dpi (logicalBaseDpi().first) 默认值 win 96/ mac 72
        const QDpi base_dpi = cur_screen->handle()->logicalBaseDpi();
        const QDpi logic_dpi = QPlatformScreen::overrideDpi(cur_screen->handle()->logicalDpi());
        const qreal factor = qreal(logic_dpi.first) / qreal(base_dpi.first);
        qDebug()<<"calc dpi"<<factor;
        setDpiRatio(factor);
    }
}
