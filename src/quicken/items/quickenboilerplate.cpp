// Copyright © 2018 Loïc Molinari <loicm@loicm.fr>
//
// This file is part of Quicken, licensed under the MIT license. See the license
// file at project root for full information.

#include "quickenboilerplate_p.h"

#include "quickenboilerplatenode_p.h"

QuickenBoilerplate::QuickenBoilerplate(QQuickItem* parent)
    : QQuickItem(parent)
{
    setFlag(ItemHasContents);
#if QT_VERSION < QT_VERSION_CHECK(6, 0, 0)
    setAcceptTouchEvents(false);
#endif
}
 
QSGNode* QuickenBoilerplate::updatePaintNode(QSGNode* oldNode, UpdatePaintNodeData* data)
{
    Q_UNUSED(data);

    const float w = width();
    const float h = height();

    if (w <= 0.0f || h <= 0.0f) {
        delete oldNode;
        return nullptr;
    }

    QuickenBoilerplateNode* node =
        oldNode ? static_cast<QuickenBoilerplateNode*>(oldNode) : new QuickenBoilerplateNode;
    node->update(w, h);

    return node;
}
