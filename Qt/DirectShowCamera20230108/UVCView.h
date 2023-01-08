#pragma once
#include <QQuickItem>
#include "UVCCore.h"

/**
 * @brief 预览相机图像
 * @author 龚建波
 * @date 2022-12-14
 * @details
 * View负责显示图像，Core负责具体的操作
 * @todo
 * 和QQuickFramebufferObject对比下
 */
class UVCView : public QQuickItem
{
    Q_OBJECT
    Q_PROPERTY(UVCCore* core READ getCore CONSTANT)
public:
    explicit UVCView(QQuickItem *parent = nullptr);
    ~UVCView();

    //通过core控制uvc相关操作
    UVCCore *getCore();

protected:
    QSGNode *updatePaintNode(QSGNode *oldNode, UpdatePaintNodeData *) override;

private:
    //接收预览图，刷新ui
    void grabberImageRecv(const QImage &image);

private:
    //UVC相机接口调用
    UVCCore *core{NULL};
    //当前缓存的渲染的图片
    QImage viewImage;
    //接收到的图片和上次预览的比size是否相同
    bool sizeChange{false};
};
