#include <QGuiApplication>
#include <QQmlApplicationEngine>
#include <QStringListModel>
#include <QSortFilterProxyModel>
#include <QtQml/QQmlContext>

int main(int argc, char *argv[])
{
    QCoreApplication::setAttribute(Qt::AA_EnableHighDpiScaling);

    QGuiApplication app(argc, argv);

    QStringList Suggestions;
    Suggestions << "Blue";
    Suggestions << "Red";
    Suggestions << "Orange";
    Suggestions << "Ass";
    Suggestions << "Apple";
    Suggestions << "Apple1";
    Suggestions << "Apple2";
    Suggestions << "中文";
    Suggestions << "中文测试";
    QStringListModel SuggestionModel;
    SuggestionModel.setStringList(Suggestions);

    QSortFilterProxyModel ProxyModel;
    ProxyModel.setSourceModel(&SuggestionModel);
    ProxyModel.setFilterCaseSensitivity(Qt::CaseInsensitive);
    ProxyModel.sort(0);

    QQmlApplicationEngine engine;
    engine.rootContext()->setContextProperty("SuggestionsModel", &ProxyModel);
    engine.load(QUrl(QStringLiteral("qrc:/main.qml")));
    if (engine.rootObjects().isEmpty())
        return -1;

    return app.exec();
}
