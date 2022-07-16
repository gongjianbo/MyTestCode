#include <QGuiApplication>
#include <QQmlApplicationEngine>
#include <qglobal.h>

int main(int argc, char *argv[])
{
    //文档https://doc.qt.io/qt-5/windows-requirements.html
    //环境变量QT_OPENGL_BUGLIST设置OpenGL黑名单
    //disable_desktopgl --不使用常规OpenGL(opengl32.dll)，防止坏的 OpenGL驱动程序使应用程序崩溃
    //disable_angle --不使用ANGLE（以及 Direct3D）
    //disable_d3d11 --在ANGLE中禁用D3D11光栅器，ANGLE默认顺序为D3D11, D3D9, WARP, reference implementation
    //disable_d3d9 --在ANGLE中禁用D3D9光栅器
    //disable_rotation --强制应用程序始终以横向运行，仅适用于desktop opengl
    //disable_program_cache --禁用在磁盘上存储着色器程序二进制文件
    //未指定时QT_OPENGL_BUGLIST，将使用内置列表
    //这通常包括一些较旧、功能较弱的图形卡disable_desktopgl，
    //以防止Qt使用其不稳定的桌面OpenGL实现，回退到ANGLE
    qputenv("QT_OPENGL_BUGLIST", ":/opengl-blacklist.json");

#if QT_VERSION < QT_VERSION_CHECK(6, 0, 0)
    QCoreApplication::setAttribute(Qt::AA_EnableHighDpiScaling);
#endif
    //QCoreApplication::setAttribute(Qt::AA_UseDesktopOpenGL);
    QGuiApplication app(argc, argv);

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
