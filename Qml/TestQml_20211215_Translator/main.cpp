#include <QGuiApplication>
#include <QQmlApplicationEngine>
#include <QQmlContext>
#include "Translator.h"
#include "ListModel.h"

int main(int argc, char *argv[])
{
#if QT_VERSION < QT_VERSION_CHECK(6, 0, 0)
    QCoreApplication::setAttribute(Qt::AA_EnableHighDpiScaling);
#endif
    QGuiApplication app(argc, argv);

    Translator::getInstance()->setLanguage(Translator::EN_US);

    QQmlApplicationEngine engine;

    QObject::connect(Translator::getInstance(), &Translator::languageChanged,
                     &engine, &QQmlEngine::retranslate);
    qmlRegisterUncreatableType<Translator>("Trans",1,0,"Translator","Translator Uncreatable");
    engine.rootContext()->setContextProperty("translator", Translator::getInstance());
    QStringList combo_model{QObject::tr("第一项"),QObject::tr("第二项"),QObject::tr("第三项")};
    engine.rootContext()->setContextProperty("comboModel", combo_model);
    engine.rootContext()->setContextProperty("listModel", new ListModel(qApp));

    const QUrl url(QStringLiteral("qrc:/main.qml"));
    QObject::connect(&engine, &QQmlApplicationEngine::objectCreated,
                     &app, [url](QObject *obj, const QUrl &objUrl) {
        if (!obj && url == objUrl)
            QCoreApplication::exit(-1);
    }, Qt::QueuedConnection);
    engine.load(url);

    return app.exec();
}
