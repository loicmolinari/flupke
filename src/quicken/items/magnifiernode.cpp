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

#include "magnifiernode_p.h"

QuickenMagnifierNode::QuickenMagnifierNode()
    : QSGGeometryNode()
    , m_geometry(attributeSet(), 4, 4, GL_UNSIGNED_SHORT) {
    DLOG("creating QuickenMagnifierNode");

    const quint16 indices[] = { 0, 1, 2, 3 };
    memcpy(m_geometry.indexData(), indices, 4 * sizeof(quint16));
    m_geometry.setDrawingMode(GL_TRIANGLE_STRIP);
    m_geometry.setIndexDataPattern(QSGGeometry::StaticPattern);
    m_geometry.setVertexDataPattern(QSGGeometry::AlwaysUploadPattern);

    setGeometry(&m_geometry);
    setMaterial(&m_material);
    setOpaqueMaterial(&m_opaqueMaterial);

    qsgnode_set_description(this, QLatin1String("quickenmagnifier"));
}

QuickenMagnifierNode::~QuickenMagnifierNode()
{
    DLOG("detroying QuickenMagnifierNode");
}

void QuickenMagnifierNode::preprocess()
{
}

void QuickenMagnifierNode::update(float width, float height)
{
    Vertex* v = reinterpret_cast<Vertex*>(m_geometry.vertexData());

    v[0].x = 0.0f;
    v[0].y = 0.0f;
    v[0].color = 0xff0000ff;
    v[1].x = width;
    v[1].y = 0.0f;
    v[1].color = 0xff00ff00;
    v[2].x = 0.0f;
    v[2].y = height;
    v[2].color = 0xffff0000;
    v[3].x = width;
    v[3].y = height;
    v[3].color = 0xffff00ff;

    markDirty(QSGNode::DirtyGeometry);
}
