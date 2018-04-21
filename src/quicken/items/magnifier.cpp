// Copyright © 2018 Loïc Molinari <loicm@loicm.fr>
//
// This file is part of Quicken.
//
// Quicken is free software: you can redistribute it and/or modify it under the
// terms of the GNU Lesser General Public License as published by the Free
// Software Foundation; version 3.
//
// Quicken is distributed in the hope that it will be useful, but WITHOUT ANY
// WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR
// A PARTICULAR PURPOSE. See the GNU Lesser General Public License for more
// details.
//
// You should have received a copy of the GNU Lesser General Public License
// along with Quicken. If not, see <http://www.gnu.org/licenses/>.

#include "magnifier_p.h"

#include "magnifiernode_p.h"

QuickenMagnifier::QuickenMagnifier(QQuickItem* parent)
    : QQuickItem(parent)
{
    setFlag(ItemHasContents);
#if QT_VERSION < QT_VERSION_CHECK(6, 0, 0)
    setAcceptTouchEvents(false);
#endif
}
 
QSGNode* QuickenMagnifier::updatePaintNode(QSGNode* oldNode, UpdatePaintNodeData* data)
{
    Q_UNUSED(data);

    const float w = width();
    const float h = height();

    if (w <= 0.0f || h <= 0.0f) {
        delete oldNode;
        return nullptr;
    }

    QuickenMagnifierNode* node =
        oldNode ? static_cast<QuickenMagnifierNode*>(oldNode) : new QuickenMagnifierNode;
    node->update(w, h);

    return node;
}
