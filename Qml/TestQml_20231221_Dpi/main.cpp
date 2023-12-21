#include <QApplication>
#include <QQmlApplicationEngine>
#include <QQuickWindow>
#include <QFontDatabase>
#include <QFont>
#include "mywindow.h"

int main(int argc, char *argv[])
{
    // 如果是 EnableHighDpiScaling，Screen.devicePixelRatio 就是 Qt 内部使用的缩放系数
    // 默认是四舍五入整数倍，加上 PassThrough 后才有非整数倍缩放值
    // 如果是 DisableHighDpiScaling，Screen.devicePixelRatio 始终为 1
    // 注意：HighDpiScaling 设置只在 Qt5 可用，而且 Qt5 和 Qt6 QWindow 内部实现也不一样
    QCoreApplication::setAttribute(Qt::AA_EnableHighDpiScaling);
    QGuiApplication::setHighDpiScaleFactorRoundingPolicy(Qt::HighDpiScaleFactorRoundingPolicy::PassThrough);
    //QCoreApplication::setAttribute(Qt::AA_DisableHighDpiScaling);
    QQuickWindow::setTextRenderType(QQuickWindow::NativeTextRendering);

    QApplication app(argc, argv);
    QFont font = app.font();
    // font.setFamily("SimSun");
    font.setPixelSize(14);
    app.setFont(font);
    qDebug()<<"app"<<font.family();

    font = QFontDatabase::systemFont(QFontDatabase::GeneralFont);
    qDebug()<<"sys"<<font.family();

    qmlRegisterType<MyQuickWindow>("Test", 2, 15, "MyWindow");

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
