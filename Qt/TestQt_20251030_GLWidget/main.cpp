#include <QApplication>
#include <QSurfaceFormat>
#include "mainwindow.h"

int main(int argc, char *argv[])
{
#if QT_VERSION < QT_VERSION_CHECK(6, 0, 0)
    QCoreApplication::setAttribute(Qt::AA_UseHighDpiPixmaps);
    QCoreApplication::setAttribute(Qt::AA_EnableHighDpiScaling);
    // Qt5中使用ANGLE模拟ES环境，Qt6移除了
    // QGuiApplication::setAttribute(Qt::AA_UseOpenGLES);
#endif
    QGuiApplication::setHighDpiScaleFactorRoundingPolicy(Qt::HighDpiScaleFactorRoundingPolicy::PassThrough);

    // qApp设置AA_UseOpenGLES后才能设置QSurfaceFormat::OpenGLES
    // QSurfaceFormat fmt = QSurfaceFormat::defaultFormat();
    // fmt.setSamples(8);
    // fmt.setVersion(3, 0);
    // fmt.setProfile(QSurfaceFormat::NoProfile);
    // fmt.setRenderableType(QSurfaceFormat::OpenGLES);
    // QSurfaceFormat::setDefaultFormat(fmt);

    QApplication app(argc, argv);
    MainWindow w;
    w.show();
    return app.exec();
}
