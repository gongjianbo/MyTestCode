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
 * 该组件作为附加属性使用，过滤/屏蔽组件的按键处理
 *
 * Qt自带的的Keys在5.12 5.13里accepted后，
 * 组件只过滤了press，click还是会触发。
 * 在5.15中可以正常过滤。
 *
 * @example
 * KeysFilter.enable: true
 * KeysFilter.acceptShortcut: true
 * KeysFilter.filterKeys: [Qt.Key_Space]
 *
 * @note
 * 本来想转发到qml界面设置accepted，但是qquickevent是私有的
 */
class KeysFilter : public QObject
{
    Q_OBJECT
    Q_PROPERTY(bool enabled READ getEnabled WRITE setEnabled NOTIFY enabledChanged)
    Q_PROPERTY(bool acceptShortcut READ getAcceptShortcut WRITE setAcceptShortcut NOTIFY acceptShortcutChanged)
    Q_PROPERTY(QList<int> filterKeys READ getFilterKeys WRITE setFilterKeys NOTIFY filterKeysChanged)
public:
    explicit KeysFilter(QObject *parent = nullptr);

    //过滤使能
    bool getEnabled() const { return enabled; }
    void setEnabled(bool enable);

    //是否允许Shortcut转发
    bool getAcceptShortcut() const { return acceptShortcut; }
    void setAcceptShortcut(bool enable);

    //过滤的键值
    QList<int> getFilterKeys() const { return filterKeys; }
    void setFilterKeys(const QList<int> &filters);

    //事件过滤
    bool eventFilter(QObject *watched, QEvent *event) override;
    //附加属性
    static KeysFilter *qmlAttachedProperties(QObject *obj);

signals:
    void enabledChanged();
    void acceptShortcutChanged();
    void filterKeysChanged();

private:
    bool enabled=true;
    bool acceptShortcut=true;
    QList<int> filterKeys;
};
QML_DECLARE_TYPE(KeysFilter)
QML_DECLARE_TYPEINFO(KeysFilter, QML_HAS_ATTACHED_PROPERTIES)
#endif // KEYSFILTER_H
