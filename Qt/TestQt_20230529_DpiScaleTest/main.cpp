#include <QApplication>
#include <QQmlApplicationEngine>
#include <QQuickWindow>
#include <QQuickStyle>
#include <QFont>
#include <QDebug>
#include "MainWindow.h"

int main(int argc, char *argv[])
{
    // 在 pro 中修改 CONF_FILE 设置，使用 conf 设置缩放（比其他缩放优先级高），无论 qt5/6 都会模糊
    // qt6 默认开启缩放，但不支持 AA_DisableHighDpiScaling 等关闭设置

#if 1
    // 关闭缩放
    // https://doc.qt.io/qt-6/highdpi.html
    // 参见文档，仅用于测试，且不支持 mac os 和 wayland 平台
    //qputenv("QT_ENABLE_HIGHDPI_SCALING", "0");
    // 另一个调试用的变量，似乎是和 HighDpiScaleFactorRoundingPolicy 叠加的，而不是全局的控制
    //qputenv("QT_SCALE_FACTOR", "1.25");
    // windows 默认是 96 dpi，mac 似乎是 72
    // AA_Use96Dpi 在 mac os 以及 qt6 似乎没效果这里用变量设置
    qputenv("QT_FONT_DPI", "96");
#else
    // 开启缩放
    // qt5 的缩放效果不太好，各种渲染问题
#   if QT_VERSION < QT_VERSION_CHECK(6, 0, 0)
    // 这是 qt6 的默认策略，而 qt5 默认是没开启缩放的
    QCoreApplication::setAttribute(Qt::AA_EnableHighDpiScaling);
    QCoreApplication::setAttribute(Qt::AA_UseHighDpiPixmaps);
    // qt5 默认 Round 四舍五入，qt6 默认 PassThrough 允许小数值
    QGuiApplication::setHighDpiScaleFactorRoundingPolicy(Qt::HighDpiScaleFactorRoundingPolicy::PassThrough);
#   endif
#endif
    // 如果是 qt5 的缩放，qml 中使用 nativetext 会有锯齿和撕裂
    QQuickWindow::setTextRenderType(QQuickWindow::NativeTextRendering);
    QQuickStyle::setStyle("Basic");

    QApplication app(argc, argv);

    qDebug() << app.font().families() << app.font().defaultFamily();
    QFont font;
    // qt5/6 默认字体不一样，宋体相对锐利能看清细节，方便测试
    font.setFamily("SimSun");
    // 不设置 pixel size，qt5 使用 96dpi 时字变小
    font.setPixelSize(13);
    app.setFont(font);

    MainWindow window;
    window.move(10, 10);
    window.show();

    QQmlApplicationEngine engine;
    const QUrl url(QStringLiteral("qrc:/main.qml"));
    QObject::connect(&engine, &QQmlApplicationEngine::objectCreated,
        &app, [url](QObject *obj, const QUrl &objUrl) {
            if (!obj && url == objUrl)
                QCoreApplication::exit(-1);
        }, Qt::QueuedConnection);
    engine.load(url);

    return app.exec();
}
