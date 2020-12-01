#ifndef KEYSFILTER_H
#define KEYSFILTER_H

#include <QQuickItem>
#include <QKeyEvent>
#include <qqml.h>

/**
 * @brief 按键过滤
 * @author 龚建波
 * @date 2020-12-1
 * @details
 * 作为附加属性使用，过滤/屏蔽组件的按键处理
 * @example
 * KeysFilter.enable: true
 * KeysFilter.filterKeys: [Qt.Key_Space]
 * @note
 * 本来想转发到qml界面设置accepted，但是qquickevent是私有的
 */
class KeysFilter : public QObject
{
    Q_OBJECT
    Q_PROPERTY(bool enable READ getEnable WRITE setEnable NOTIFY enableChanged)
    Q_PROPERTY(QList<int> filterKeys READ getFilterKeys WRITE setFilterKeys NOTIFY filterKeysChanged)
public:
    explicit KeysFilter(QObject *parent = nullptr);

    //过滤使能
    bool getEnable() const { return enable; }
    void setEnable(bool enable);

    //过滤的键值
    QList<int> getFilterKeys() const { return filterKeys; }
    void setFilterKeys(const QList<int> &filters);

    //事件过滤
    bool eventFilter(QObject *watched, QEvent *event) override;
    //附加属性
    static KeysFilter *qmlAttachedProperties(QObject *obj);

signals:
    void enableChanged();
    void filterKeysChanged();

private:
    bool enable=true;
    QList<int> filterKeys;
};
QML_DECLARE_TYPE(KeysFilter)
QML_DECLARE_TYPEINFO(KeysFilter, QML_HAS_ATTACHED_PROPERTIES)
#endif // KEYSFILTER_H
