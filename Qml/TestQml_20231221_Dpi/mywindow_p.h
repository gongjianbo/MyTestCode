#pragma once
#include "mywindow.h"
#include <qquickrendercontrol.h>
#include <private/qquickscreen_p.h>
#include <private/qquickview_p.h>
#include <QtQuick/QQuickWindow>
#include <QtCore/QCoreApplication>
#include <QtQml/QQmlEngine>

#include <private/qguiapplication_p.h>
#include <private/qqmlengine_p.h>
#include <private/qv4qobjectwrapper_p.h>
#include <private/qqmlglobal_p.h>
#include <qpa/qplatformintegration.h>


class MyQuickWindowPrivate : public QQuickWindowPrivate
{
public:
    MyQuickWindowPrivate()
        : complete(false)
        , visible(false)
        , visibility(QQuickWindow::AutomaticVisibility)
    {
    }

    bool complete;
    bool visible;
    qreal dpiRatio;
    QMetaObject::Connection physicalDpiConnection;
    QMetaObject::Connection logicalDpiConnection;
    QQuickWindow::Visibility visibility;
    QV4::PersistentValue rootItemMarker;
};

