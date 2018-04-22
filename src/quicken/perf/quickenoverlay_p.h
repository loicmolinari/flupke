// Copyright © 2017-2018 Loïc Molinari <loicm@loicm.fr>
// Copyright © 2016 Canonical Ltd.
//
// This file is part of Quicken, licensed under the MIT license. See the license
// file at project root for full information.

#ifndef OVERLAY_P_H
#define OVERLAY_P_H

#include <QtCore/QSize>

#include <Quicken/quickenmetrics.h>
#include <Quicken/private/quickenbitmaptext_p.h>
#include <Quicken/private/quickenglobal_p.h>

#if !defined QT_NO_DEBUG
class QOpenGLContext;
#endif

// Renders an overlay based on various metrics.
class QUICKEN_PRIVATE_EXPORT QuickenOverlay
{
public:
    QuickenOverlay(const char* text, int windowId);
    ~QuickenOverlay();

    // Allocates/Deletes the OpenGL resources. finalize() is not called at
    // destruction, it must be explicitly called to free the resources at the
    // right time in a thread with the same OpenGL context bound than at
    // initialize().
    bool initialize();
    void finalize();

    // Sets the process metrics.
    void setProcessMetrics(const QuickenMetrics& processMetrics);

    // Renders the overlay. Must be called in a thread with the same OpenGL
    // context bound than at initialize().
    void render(const QuickenMetrics& frameMetrics, const QSize& frameSize);

private:
    void updateFrameMetrics(const QuickenMetrics& frameMetrics);
    void updateWindowMetrics(quint32 windowId, const QSize& frameSize);
    void updateProcessMetrics();
    int keywordString(int index, char* buffer, int bufferSize);
    void parseText();

    enum {
        Initialized         = (1 << 0),
        DirtyText           = (1 << 1),
        DirtyProcessMetrics = (1 << 2)
    };

    static const int maxMetricsPerType = 16;

    void* m_buffer;
    char* m_parsedText;
#if !defined QT_NO_DEBUG
    QOpenGLContext* m_context;
#endif
    QString m_text;
    struct {
        quint16 index;
        quint16 textIndex;
        quint8 width;
    } m_metrics[QuickenMetrics::TypeCount][maxMetricsPerType];
    quint8 m_metricsSize[QuickenMetrics::TypeCount];
    QuickenBitmapText m_bitmapText;
    QSize m_frameSize;
    quint32 m_windowId;
    quint8 m_flags;
    alignas(64) QuickenMetrics m_processMetrics;
};

#endif  // OVERLAY_P_H
