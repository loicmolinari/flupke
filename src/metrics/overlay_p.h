// Copyright © 2017 Loïc Molinari <loicm@loicm.fr>
// Copyright © 2016 Canonical Ltd.
//
// This file is part of Flupke.
//
// Flupke is free software: you can redistribute it and/or modify it under the
// terms of the GNU Lesser General Public License as published by the Free
// Software Foundation; version 3.
//
// Flupke is distributed in the hope that it will be useful, but WITHOUT ANY
// WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR
// A PARTICULAR PURPOSE. See the GNU Lesser General Public License for more
// details.
//
// You should have received a copy of the GNU Lesser General Public License
// along with Flupke. If not, see <http://www.gnu.org/licenses/>.

#ifndef OVERLAY_P_H
#define OVERLAY_P_H

#include <QtCore/QSize>

#include <FlupkeMetrics/events.h>
#include <FlupkeMetrics/private/bitmaptext_p.h>
#include <FlupkeMetrics/private/flupkemetricsglobal_p.h>

#if !defined QT_NO_DEBUG
class QOpenGLContext;
#endif

// Renders an overlay based on various metrics.
class FLUPKE_METRICS_PRIVATE_EXPORT FMOverlay
{
public:
    FMOverlay(const char* text, int windowId);
    ~FMOverlay();

    // Allocates/Deletes the OpenGL resources. finalize() is not called at
    // destruction, it must be explicitly called to free the resources at the
    // right time in a thread with the same OpenGL context bound than at
    // initialize().
    bool initialize();
    void finalize();

    // Sets the process event.
    void setProcessEvent(const FMEvent& processEvent);

    // Renders the overlay. Must be called in a thread with the same OpenGL
    // context bound than at initialize().
    void render(const FMEvent& frameEvent, const QSize& frameSize);

private:
    void updateFrameMetrics(const FMEvent& frameEvent);
    void updateWindowMetrics(quint32 windowId, const QSize& frameSize);
    void updateProcessMetrics();
    int keywordString(int index, char* buffer, int bufferSize);
    void parseText();

    enum {
        Initialized       = (1 << 0),
        DirtyText         = (1 << 1),
        DirtyProcessEvent = (1 << 2)
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
    } m_metrics[FMEvent::TypeCount][maxMetricsPerType];
    quint8 m_metricsSize[FMEvent::TypeCount];
    FMBitmapText m_bitmapText;
    QSize m_frameSize;
    quint32 m_windowId;
    quint8 m_flags;
    alignas(64) FMEvent m_processEvent;
};

#endif  // OVERLAY_P_H
