#include "ScreenTool.h"
#include <QGuiApplication>
#include <QWindow>

QRect ScreenTool::focusRect()
{
    QScreen *screen{nullptr};
    auto &&window = qApp->focusWindow();
    if (window) {
        screen = window->screen();
    } else {
        screen = qApp->primaryScreen();
    }
    if (screen) {
        return screen->availableGeometry();
    }
    return QRect();
}
