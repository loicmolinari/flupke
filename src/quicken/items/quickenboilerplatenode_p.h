// Copyright © 2018 Loïc Molinari <loicm@loicm.fr>
//
// This file is part of Quicken, licensed under the MIT license. See the license
// file at project root for full information.

#ifndef BOILERPLATENODE_P_H
#define BOILERPLATENODE_P_H

#include <QtQuick/QSGMaterial>
#include <QtQuick/QSGNode>

#include <Quicken/private/quickenglobal_p.h>

class QUICKEN_PRIVATE_EXPORT QuickenBoilerplateOpaqueShader : public QSGMaterialShader
{
public:
    QuickenBoilerplateOpaqueShader() {
        setShaderSourceFile(QOpenGLShader::Vertex,
                            QStringLiteral(":/quicken/shaders/boilerplate.vert"));
        setShaderSourceFile(QOpenGLShader::Fragment,
                            QStringLiteral(":/quicken/shaders/boilerplateopaque.frag"));
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

class QUICKEN_PRIVATE_EXPORT QuickenBoilerplateShader : public QuickenBoilerplateOpaqueShader
{
public:
    QuickenBoilerplateShader() : QuickenBoilerplateOpaqueShader() {
        setShaderSourceFile(QOpenGLShader::Fragment,
                            QStringLiteral(":/quicken/shaders/boilerplate.frag"));
    }
    void initialize() Q_DECL_OVERRIDE {
        QuickenBoilerplateOpaqueShader::initialize();
        m_opacityId = program()->uniformLocation("opacity");
    }
    void updateState(
        const RenderState& state, QSGMaterial* newEffect, QSGMaterial* oldEffect) Q_DECL_OVERRIDE {
        QuickenBoilerplateOpaqueShader::updateState(state, newEffect, oldEffect);
        if (state.isOpacityDirty()) {
            program()->setUniformValue(m_opacityId, state.opacity());
        }
    }

private:
    int m_opacityId;
};

template <bool opaque>
class QUICKEN_PRIVATE_EXPORT QuickenBoilerplateMaterial : public QSGMaterial
{
public:
    QuickenBoilerplateMaterial() {
        setFlag(Blending, !opaque);
    }

    QSGMaterialType* type() const Q_DECL_OVERRIDE {
        static QSGMaterialType type[2];
        return opaque ? &type[0] : &type[1];
    }
    QSGMaterialShader* createShader() const Q_DECL_OVERRIDE {
        return opaque ? new QuickenBoilerplateOpaqueShader : new QuickenBoilerplateShader;
    }
    int compare(const QSGMaterial* other) const Q_DECL_OVERRIDE {
        Q_UNUSED(other);
        return 0;
    }

private:
};

class QUICKEN_PRIVATE_EXPORT QuickenBoilerplateNode : public QSGGeometryNode
{
public:
    QuickenBoilerplateNode();
    ~QuickenBoilerplateNode();

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

    QuickenBoilerplateMaterial<false> m_material;
    QuickenBoilerplateMaterial<true> m_opaqueMaterial;
    QSGGeometry m_geometry;
};

#endif  // BOILERPLATENODE_P_H
