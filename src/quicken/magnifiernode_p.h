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

#ifndef MAGNIFIERNODE_P_H
#define MAGNIFIERNODE_P_H

#include <QtQuick/QSGMaterial>
#include <QtQuick/QSGNode>

#include <Quicken/private/quickenglobal_p.h>

class QUICKEN_PRIVATE_EXPORT QcknMagnifierOpaqueShader : public QSGMaterialShader
{
public:
    QcknMagnifierOpaqueShader() {
        setShaderSourceFile(QOpenGLShader::Vertex,
                            QStringLiteral(":/quicken/shaders/magnifier.vert"));
        setShaderSourceFile(QOpenGLShader::Fragment,
                            QStringLiteral(":/quicken/shaders/magnifieropaque.frag"));
    }
    char const* const* attributeNames() const Q_DECL_OVERRIDE {
        static char const* const attributes[] = {
            "positionAttrib", "colorAttrib", 0
        };
        return attributes;
    }
    void initialize() Q_DECL_OVERRIDE {
        QSGMaterialShader::initialize();
        m_matrixId = program()->uniformLocation("matrix");
    }
    void updateState(
        const RenderState& state, QSGMaterial*, QSGMaterial*) Q_DECL_OVERRIDE {
        if (state.isMatrixDirty()) {
            program()->setUniformValue(m_matrixId, state.combinedMatrix());
        }
    }

private:
    int m_matrixId;
};

class QUICKEN_PRIVATE_EXPORT QcknMagnifierShader : public QcknMagnifierOpaqueShader
{
public:
    QcknMagnifierShader() : QcknMagnifierOpaqueShader() {
        setShaderSourceFile(QOpenGLShader::Fragment,
                            QStringLiteral(":/quicken/shaders/magnifier.frag"));
    }
    void initialize() Q_DECL_OVERRIDE {
        QcknMagnifierOpaqueShader::initialize();
        m_opacityId = program()->uniformLocation("opacity");
    }
    void updateState(
        const RenderState& state, QSGMaterial* newEffect, QSGMaterial* oldEffect) Q_DECL_OVERRIDE {
        QcknMagnifierOpaqueShader::updateState(state, newEffect, oldEffect);
        if (state.isOpacityDirty()) {
            program()->setUniformValue(m_opacityId, state.opacity());
        }
    }

private:
    int m_opacityId;
};

template <bool opaque>
class QUICKEN_PRIVATE_EXPORT QcknMagnifierMaterial : public QSGMaterial
{
public:
    QcknMagnifierMaterial() {
        setFlag(Blending, !opaque);
    }

    QSGMaterialType* type() const Q_DECL_OVERRIDE {
        static QSGMaterialType type[2];
        return opaque ? &type[0] : &type[1];
    }
    QSGMaterialShader* createShader() const Q_DECL_OVERRIDE {
        return opaque ? new QcknMagnifierOpaqueShader : new QcknMagnifierShader;
    }
    int compare(const QSGMaterial* other) const Q_DECL_OVERRIDE {
        Q_UNUSED(other);
        return 0;
    }

private:
};

class QUICKEN_PRIVATE_EXPORT QcknMagnifierNode : public QSGGeometryNode
{
public:
    QcknMagnifierNode();
    ~QcknMagnifierNode();

    void preprocess() Q_DECL_OVERRIDE;

    void update(float width, float height);

private:
    struct Vertex {
        float x, y;
        quint32 color;
    };

    static const QSGGeometry::AttributeSet& attributeSet() {
        static const QSGGeometry::Attribute attributes[] = {
            QSGGeometry::Attribute::create(0, 2, GL_FLOAT, true),   // x, y
            QSGGeometry::Attribute::create(1, 4, GL_UNSIGNED_BYTE)  // color
        };
        static const QSGGeometry::AttributeSet attributeSet = {
            2, sizeof(Vertex), attributes
        };
        return attributeSet;
    }

    QcknMagnifierMaterial<false> m_material;
    QcknMagnifierMaterial<true> m_opaqueMaterial;
    QSGGeometry m_geometry;
};

#endif  // MAGNIFIERNODE_P_H
