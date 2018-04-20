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

#ifndef MAGNIFIER_P_H
#define MAGNIFIER_P_H

#include <QtQuick/QQuickItem>

#include <private/quickenglobal_p.h>

class QUICKEN_PRIVATE_EXPORT QcknMagnifier : public QQuickItem
{
    Q_OBJECT

public:
    QcknMagnifier(QQuickItem* parent = Q_NULLPTR);

protected:
    QSGNode* updatePaintNode(QSGNode* oldNode, UpdatePaintNodeData* data) Q_DECL_OVERRIDE;

private:
    Q_DISABLE_COPY(QcknMagnifier)
};

QML_DECLARE_TYPE(QcknMagnifier)

#endif  // MAGNIFIER_P_H
