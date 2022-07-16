#pragma once
#include <QObject>

/**
 * @brief 按键事件过滤
 * @author 龚建波
 * @date 2021-05-17
 *
 * @details
 * 主要用于区域需要快捷键时过滤全局快捷键
 * 避免被全局 Shortcut、Action 获取
 * （触发快捷键后 Keys 就不会往上传递）
 * 过滤快捷键后可以正常触发 activeFocus 的 Keys.onPressed
 * @note
 * 如果只是单个组件请用 Keys.onShortcutOverride
 * @example
 * FocusScope {
 *     id: keys_scope
 *     anchors.fill: parent
 *     KeysFilter {
 *         id: keys_filter
 *         enabled: keys_scope.activeFocus
 *         //acceptAutoRepeat: false
 *         //filterType: KeysFilter.All
 *         filterKeys: [Qt.Key_Return,Qt.Key_Enter]
 *     }
 * }
 */
class KeysFilter : public QObject
{
    Q_OBJECT
    Q_PROPERTY(bool enabled READ getEnabled WRITE setEnabled NOTIFY enabledChanged)
    Q_PROPERTY(bool acceptAutoRepeat READ getAcceptAutoRepeat WRITE setAcceptAutoRepeat NOTIFY acceptAutoRepeatChanged)
    Q_PROPERTY(KeysFilter::FilterType filterType READ getFilterType WRITE setFilterType NOTIFY filterTypeChanged)
    Q_PROPERTY(QList<int> filterKeys READ getFilterKeys WRITE setFilterKeys NOTIFY filterKeysChanged)
public:
    //过滤类型
    enum FilterType {
        None = 0x00
        ,ShortcutOverride = 0x01
        ,KeyPress = 0x02
        ,KeyRelease = 0x04
        ,All = 0xFFFF
    };
    Q_ENUM(FilterType)
public:
    explicit KeysFilter(QObject *parent = nullptr);
    //bool event(QEvent *e) override;
    bool eventFilter(QObject *watched, QEvent *event) override;

    //使能-eventFilter
    bool getEnabled() const { return enabled; }
    void setEnabled(bool enable);

    //使能-长按重复触发
    bool getAcceptAutoRepeat() const { return acceptAutoRepeat; }
    void setAcceptAutoRepeat(bool accept);

    //过滤类型，默认过滤快捷键
    FilterType getFilterType() const { return filterType; }
    void setFilterType(FilterType type);

    //接受的按键列表
    QList<int> getFilterKeys() const { return filterKeys; }
    void setFilterKeys(const QList<int> &keys);

signals:
    void enabledChanged();
    void acceptAutoRepeatChanged();
    void filterTypeChanged();
    void filterKeysChanged();
    //过滤的按键触发press
    void pressed(int key);
    //过滤的按键触发release
    void released(int key);

private:
    //使能-eventFilter
    bool enabled = true;
    //使能-长按重复触发
    bool acceptAutoRepeat = false;
    //过滤类型
    FilterType filterType = FilterType::ShortcutOverride;
    //过滤的按键列表
    QList<int> filterKeys;
};
