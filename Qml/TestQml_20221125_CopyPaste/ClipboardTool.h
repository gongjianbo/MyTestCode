#pragma once
#include <QObject>
#include <QUrl>

//剪切板相关操作
class ClipboardTool : public QObject
{
    Q_OBJECT
public:
    explicit ClipboardTool(QObject *parent = nullptr);

    //当前剪切板的text
    Q_INVOKABLE QString currentText();
    //当前剪切板的text转为url
    Q_INVOKABLE QUrl currentUrl();
    //设置剪切版的图片url
    Q_INVOKABLE void setCurrentUrl(const QUrl &url);
};
