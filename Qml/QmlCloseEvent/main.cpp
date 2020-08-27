#include <QApplication>
#include <QQmlApplicationEngine>
#include <QMessageBox>
#include <QDebug>

int main(int argc, char *argv[])
{
    QCoreApplication::setAttribute(Qt::AA_EnableHighDpiScaling);

    QApplication app(argc, argv);

    QQmlApplicationEngine engine;
    const QUrl url(QStringLiteral("qrc:/main.qml"));
    QObject::connect(&engine, &QQmlApplicationEngine::objectCreated,
                     &app, [url](QObject *obj, const QUrl &objUrl) {
        if (!obj && url == objUrl)
            QCoreApplication::exit(-1);
    }, Qt::QueuedConnection);

    QObject::disconnect(&engine,&QQmlApplicationEngine::quit,nullptr,nullptr);
    QObject::disconnect(&engine,&QQmlApplicationEngine::exit,nullptr,nullptr);
    QObject::connect(&engine,&QQmlApplicationEngine::quit,[]{
        qDebug()<<"quit";
        if(QMessageBox::warning(nullptr,"提示","确认退出？",
                             QMessageBox::Ok|QMessageBox::Cancel)==QMessageBox::Ok)
            qApp->quit();
    });
    QObject::connect(&engine,&QQmlApplicationEngine::exit,[](int code){
        qDebug()<<"exit"<<code;
        if(QMessageBox::warning(nullptr,"提示","确认退出？",
                             QMessageBox::Ok|QMessageBox::Cancel)==QMessageBox::Ok)
            qApp->quit();
    });

    engine.load(url);

    return app.exec();
}
