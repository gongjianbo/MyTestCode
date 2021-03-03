#include <QGuiApplication>
#include <QQmlApplicationEngine>
#include <QQmlComponent>
#include <QDebug>

void updateToolTip(QQmlApplicationEngine *engine);

int main(int argc, char *argv[])
{
    QCoreApplication::setAttribute(Qt::AA_EnableHighDpiScaling);

    QGuiApplication app(argc, argv);

    QQmlApplicationEngine engine;

    const QUrl url(QStringLiteral("qrc:/main.qml"));
    QObject::connect(&engine, &QQmlApplicationEngine::objectCreated,
                     &app, [url](QObject *obj, const QUrl &objUrl) {
        if (!obj && url == objUrl)
            QCoreApplication::exit(-1);
    }, Qt::QueuedConnection);
    engine.load(url);
    updateToolTip(&engine);

    return app.exec();
}

void updateToolTip(QQmlApplicationEngine *engine)
{
    //替换默认的ToolTip附加属性样式
    static const char *name = "_q_QQuickToolTip";
    QQmlComponent *component = new QQmlComponent(engine,QUrl("qrc:/BasicToolTip.qml"));
    auto create_func = [component,engine]{
        if(component->isError()){
            qWarning() << "create BasicToolTip failed." << component->errors();
        }else if(component->isReady()){
            QObject *object = component->create();
            if (object){
                object->setParent(engine);
                engine->setProperty(name, QVariant::fromValue(object));
            }
        }
    };
    if(component->isLoading()){
        QObject::connect(component,&QQmlComponent::statusChanged,create_func);
    }else{
        create_func();
    }
}
