#pragma once
#include <QScreen>

/**
 * @brief 屏幕信息工具类
 * @author 龚建波
 * @date 2024-06-22
 */
class ScreenTool : public QObject
{
    Q_OBJECT
public:
    using QObject::QObject;

    // 焦点所在屏幕的位置和大小
    Q_INVOKABLE QRect focusRect();
};
