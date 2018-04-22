// Copyright © 2018 Loïc Molinari <loicm@loicm.fr>
//
// This file is part of Quicken, licensed under the MIT license. See the license
// file at project root for full information.

#ifndef BOILERPLATE_P_H
#define BOILERPLATE_P_H

#include <QtQuick/QQuickItem>

#include <Quicken/private/quickenglobal_p.h>

class QUICKEN_PRIVATE_EXPORT QuickenBoilerplate : public QQuickItem
{
    Q_OBJECT

public:
    QuickenBoilerplate(QQuickItem* parent = Q_NULLPTR);

protected:
    QSGNode* updatePaintNode(QSGNode* oldNode, UpdatePaintNodeData* data) Q_DECL_OVERRIDE;

private:
    Q_DISABLE_COPY(QuickenBoilerplate)
};

QML_DECLARE_TYPE(QuickenBoilerplate)

#endif  // BOILERPLATE_P_H
