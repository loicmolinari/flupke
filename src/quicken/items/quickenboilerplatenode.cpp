// Copyright © 2018 Loïc Molinari <loicm@loicm.fr>
//
// This file is part of Quicken, licensed under the MIT license. See the license
// file at project root for full information.

#include "quickenboilerplatenode_p.h"

QuickenBoilerplateNode::QuickenBoilerplateNode()
    : QSGGeometryNode()
    , m_geometry(attributeSet(), 4, 4, GL_UNSIGNED_SHORT) {
    DLOG("creating QuickenBoilerplateNode");

    const quint16 indices[] = { 0, 1, 2, 3 };
    memcpy(m_geometry.indexData(), indices, 4 * sizeof(quint16));
    m_geometry.setDrawingMode(GL_TRIANGLE_STRIP);
    m_geometry.setIndexDataPattern(QSGGeometry::StaticPattern);
    m_geometry.setVertexDataPattern(QSGGeometry::AlwaysUploadPattern);

    setGeometry(&m_geometry);
    setMaterial(&m_material);
    setOpaqueMaterial(&m_opaqueMaterial);

    qsgnode_set_description(this, QLatin1String("quickenboilerplate"));
}

QuickenBoilerplateNode::~QuickenBoilerplateNode()
{
    DLOG("detroying QuickenBoilerplateNode");
}

void QuickenBoilerplateNode::preprocess()
{
}

void QuickenBoilerplateNode::update(float width, float height)
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
