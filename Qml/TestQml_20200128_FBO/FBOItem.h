#ifndef FBOITEM_H
#define FBOITEM_H

#include <QtQuick/QQuickFramebufferObject>

class FBOItem : public QQuickFramebufferObject
{
    Q_OBJECT
public:
    FBOItem(QQuickItem *parent = nullptr);

    Renderer *createRenderer() const override;
};

#endif // FBOITEM_H
