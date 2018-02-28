// Copyright © 2017-2018 Loïc Molinari <loicm@loicm.fr>
// Copyright © 2016 Canonical Ltd.
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

#ifndef OVERLAY_P_H
#define OVERLAY_P_H

#include <QtCore/QSize>

#include <QuickenPerf/metrics.h>
#include <QuickenPerf/private/bitmaptext_p.h>
#include <QuickenPerf/private/quickenperfglobal_p.h>

#if !defined QT_NO_DEBUG
class QOpenGLContext;
#endif

// Renders an overlay based on various metrics.
class QUICKEN_PERF_PRIVATE_EXPORT QPOverlay
{
public:
    QPOverlay(const char* text, int windowId);
    ~QPOverlay();

    // Allocates/Deletes the OpenGL resources. finalize() is not called at
    // destruction, it must be explicitly called to free the resources at the
    // right time in a thread with the same OpenGL context bound than at
    // initialize().
    bool initialize();
    void finalize();

    // Sets the process metrics.
    void setProcessMetrics(const QPMetrics& processMetrics);

    // Renders the overlay. Must be called in a thread with the same OpenGL
    // context bound than at initialize().
    void render(const QPMetrics& frameMetrics, const QSize& frameSize);

private:
    void updateFrameMetrics(const QPMetrics& frameMetrics);
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
    } m_metrics[QPMetrics::TypeCount][maxMetricsPerType];
    quint8 m_metricsSize[QPMetrics::TypeCount];
    QPBitmapText m_bitmapText;
    QSize m_frameSize;
    quint32 m_windowId;
    quint8 m_flags;
    alignas(64) QPMetrics m_processMetrics;
};

#endif  // OVERLAY_P_H
