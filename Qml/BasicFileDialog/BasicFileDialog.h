#pragma once
#include <QObject>
#include <QUrl>
class QFileDialog;

/**
 * @brief 包装QFileDialog在QML中使用
 * @author 龚建波
 * @date 2022-12-31
 * @details
 * 参考：https://doc.qt.io/qt-5/qtquick-performance.html
 * 文档指出我们不应该在qml逻辑中执行自己的事件循环，所以通过信号槽异步调用对话框exec
 * 而操作结果也只能通过信号传递，不能像cpp代码那样exec后面直接获取结果
 */
class BasicFileDialog : public QObject
{
    Q_OBJECT
    Q_PROPERTY(int mode READ getMode WRITE setMode NOTIFY modeChanged)
    Q_PROPERTY(QString title READ getTitle WRITE setTitle NOTIFY titleChanged)
    Q_PROPERTY(QStringList nameFilters READ getNameFilters WRITE setNameFilters NOTIFY nameFiltersChanged)
    Q_PROPERTY(QString selectedNameFilter READ getSelectedNameFilter WRITE setSelectedNameFilter NOTIFY selectedNameFilterChanged)
    Q_PROPERTY(QUrl currentUrl READ getCurrentUrl NOTIFY currentUrlChanged)
    Q_PROPERTY(QList<QUrl> currentUrls READ getCurrentUrls NOTIFY currentUrlsChanged)
public:
    enum WorkMode {
        SelectFile // 单选文件
        , SelectFiles // 多选文件
        , SelectDir // 选择目录
        , SaveFile // 保存文件
    };
    Q_ENUM(WorkMode)
public:
    explicit BasicFileDialog(QObject *parent = nullptr);
    ~BasicFileDialog();

    // 工作模式，枚举WorkMode
    int getMode() const;
    void setMode(int mode);

    // 标题设置
    QString getTitle() const;
    void setTitle(const QString &title);

    // 文件格式后缀过滤
    QStringList getNameFilters() const;
    void setNameFilters(const QStringList &filters);

    // 选中的格式后缀
    QString getSelectedNameFilter() const;
    void setSelectedNameFilter(const QString &filter);

    // 单选结果
    QUrl getCurrentUrl() const;
    void setCurrentUrl(const QUrl &url);

    // 多选结果
    QList<QUrl> getCurrentUrls() const;
    void setCurrentUrls(const QList<QUrl> &urls);

    Q_INVOKABLE void show();
    Q_INVOKABLE void open();
    Q_INVOKABLE void close();
    // 打开前选中某个路径，带文件名则作为选中的文件名
    Q_INVOKABLE void selectUrl(const QUrl &url);
    Q_INVOKABLE void selectPath(const QString &path);

    // 桌面路径
    Q_INVOKABLE static QString desktopPath();
    Q_INVOKABLE static QUrl desktopUrl();
    // 上次的路径
    Q_INVOKABLE static QString lastPath();
    Q_INVOKABLE static QUrl lastUrl();

private:
    static QString getDefaultPath();
    static void setDefaultPath(const QString &dirpath);

private slots:
    void openDialog();

signals:
    void __openRequest();
    void __closeRequest();
    void accepted();
    void rejected();
    void closing();

    void modeChanged();
    void titleChanged();
    void nameFiltersChanged();
    void selectedNameFilterChanged();
    void currentUrlChanged();
    void currentUrlsChanged();

private:
    // 对话框
    QFileDialog *dialog;
    // 模式，选择文件还是目录等
    int workMode{BasicFileDialog::SelectFile};
    // 文件格式后缀过滤
    QStringList nameFilters{"All (*.*)"};
    // 选中的格式后缀
    QString selectedNameFilter;
    // 单选结果，当前选中路径
    QUrl currentUrl;
    // 多选结果
    QList<QUrl> currentUrls;
    // selectUrl可设置，下次弹出时生效
    QUrl nextUrl;
};
