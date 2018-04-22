// Copyright © 2017-2018 Loïc Molinari <loicm@loicm.fr>
// Copyright © 2016 Canonical Ltd.
//
// This file is part of Quicken, licensed under the MIT license. See the license
// file at project root for full information.

#ifndef BITMAPTEXT_P_H
#define BITMAPTEXT_P_H

#include <QtGui/QOpenGLFunctions>

#include <Quicken/private/quickenglobal_p.h>

// QuickenBitmapText renders a monospaced bitmap Latin-1 encoded text (128
// characters) stored in a single texture atlas using OpenGL. The font is
// generated by bitmap-text-builder and stored in the bitmaptextfont_p.h header.
class QUICKEN_PRIVATE_EXPORT QuickenBitmapText
{
public:
    QuickenBitmapText();
    ~QuickenBitmapText();

    // Allocates/Deletes the OpenGL resources. finalize() is not called at
    // destruction, it must be explicitly called to free the resources at the
    // right time in a thread with the same OpenGL context bound than at
    // initialize().
    bool initialize();
    void finalize();

    // Sets the text. Characters below 32 and above 126 included are ignored
    // apart from line feeds (10). Implies a reallocation of internal data. Must
    // be called in a thread with the same OpenGL context bound than at
    // initialize().
    void setText(const char* text);

    // Updates the current text at the given index. In order to avoid expensive
    // layout updates, line feeds can't be added nor removed. Updates of
    // characters below 32 and above 126 in the new text are ignored.
    void updateText(const char* text, int index, int length);

    // Binds the QuickenBitmapText's shader program. Must be called prior to
    // setTransform, setOpacity and render calls.
    void bindProgram();

    // Sets the viewport size and text position. Origin is at top/left. Must be
    // set correctly prior to rendering for correct results. Must be called in a
    // thread with the same OpenGL context bound than at initialize().
    void setTransform(const QSize& viewportSize, const QPointF& position);

    // Sets the text opacity. Must be called in a thread with the same OpenGL
    // context bound than at initialize().
    void setOpacity(float opacity);

    // Renders the text. Must be called in a thread with the same OpenGL context
    // bound than at initialize().
    void render();

private:
    struct Vertex {
        float x, y, s, t;
    };
    enum {
        NotEmpty    = (1 << 0),
#if !defined(QT_NO_DEBUG)
        Initialized = (1 << 1)
#endif
    };

    QOpenGLFunctions* m_functions;
#if !defined QT_NO_DEBUG
    QOpenGLContext* m_context;
#endif
    Vertex* m_vertexBuffer;
    int* m_textToVertexBuffer;
    int m_textLength;
    int m_characterCount;
    int m_currentFont;
    GLuint m_program;
    GLint m_programTransform;
    GLint m_programOpacity;
    GLuint m_vertexShaderObject;
    GLuint m_fragmentShaderObject;
    GLuint m_texture;
    GLuint m_indexBuffer;
    quint8 m_flags;
};

#endif  // BITMAPTEXT_P_H
