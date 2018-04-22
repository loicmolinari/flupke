// Copyright © 2017-2018 Loïc Molinari <loicm@loicm.fr>
// Copyright © 2016 Canonical Ltd.
//
// This file is part of Quicken, licensed under the MIT license. See the license
// file at project root for full information.

#ifndef METRICS_H
#define METRICS_H

#include <Quicken/quickenglobal.h>

// FIXME(loicm) Using alignas() in that public header requires users to have a
//     C++11 compiler. That also makes the use of Q_NULLPTR and Q_OVERRIDE in
//     other public headers useless.

struct QUICKEN_EXPORT QuickenProcessMetrics
{
    // Virtual size of the process in kilobytes.
    quint32 vszMemory;

    // Resident set size (RSS) of the process in kilobytes.
    quint32 rssMemory;

    // CPU usage of the process as a percentage. 100%, for instance, if all the
    // cores are at 100% usage, 50% if half of the cores are at 100%.
    quint16 cpuUsage;

    // Number of threads at buffer swap.
    quint16 threadCount;

    // The whole struct must take 112 bytes to allow future additions and best
    // memory alignment, don't forget to update when adding new metrics.
    quint8 __reserved[/*12 bytes taken,*/ 100 /*bytes free*/];
};
Q_STATIC_ASSERT(sizeof(QuickenProcessMetrics) == 112);

struct QUICKEN_EXPORT QuickenWindowMetrics
{
    enum State { Hidden = 0, Shown = 1, Resized = 2, StateCount = 3 };

    // Window id.
    quint32 id;

    // Width of the window.
    quint16 width;

    // Height of the window.
    quint16 height;

    // State of the window.
    State state : 8;

    // The whole struct must take 112 bytes to allow future additions and best
    // memory alignment, don't forget to update when adding new metrics.
    quint8 __reserved[/*9 bytes taken,*/ 103 /*bytes free*/];
};
Q_STATIC_ASSERT(sizeof(QuickenWindowMetrics) == 112);

struct QUICKEN_EXPORT QuickenFrameMetrics
{
    // The id of the window on which the frame has been rendered.
    quint32 window;

    // The frame number. Corresponds to the number of frames rendered after
    // scene graph initialisation.
    quint32 number;

    // Time in nanoseconds since the last frame swap.
    quint64 deltaTime;

    // Time in nanoseconds taken by the QtQuick scene graph synchronization
    // pass.
    quint64 syncTime;

    // Time in nanoseconds taken by the QtQuick scene graph render pass.
    quint64 renderTime;

    // Time in nanoseconds taken by the GPU to execute the graphics commands
    // pushed during the QtQuick scene graph render pass.
    quint64 gpuTime;

    // Time in nanoseconds taken by the graphics subsystem's buffer swap call.
    quint64 swapTime;

    // The whole struct must take 112 bytes to allow future additions and best
    // memory alignment, don't forget to update when adding new metrics.
    quint8 __reserved[/*48 bytes taken,*/ 64 /*bytes free*/];
};
Q_STATIC_ASSERT(sizeof(QuickenFrameMetrics) == 112);

struct QUICKEN_EXPORT QuickenGenericMetrics
{
    static const quint32 maxStringSize = 64;

    // Id retrieved from QuickenApplicationMonitor::registerGenericMetrics().
    quint32 id;

    // Size of the string (including the null-terminating char).
    quint32 stringSize;

    // Null-terminated string describing the generic metrics.
    char string[maxStringSize];

    // The whole struct must take 112 bytes to allow future additions and best
    // memory alignment, don't forget to update when adding new metrics.
    quint8 __reserved[/*72 bytes taken,*/ 40 /*bytes free*/];
};
Q_STATIC_ASSERT(sizeof(QuickenGenericMetrics) == 112);

struct QUICKEN_EXPORT QuickenMetrics
{
    enum Type { Process = 0, Window = 1, Frame = 2, Generic = 3, TypeCount = 4 };

    // Metrics type.
    Type type;

    // Time stamp in nanoseconds.
    alignas(8) quint64 timeStamp;

    union {
        QuickenProcessMetrics process;
        QuickenWindowMetrics window;
        QuickenFrameMetrics frame;
        QuickenGenericMetrics generic;
    };
};
Q_STATIC_ASSERT(sizeof(QuickenMetrics) == 128);

class QuickenMetricsUtilsPrivate;

// Utilities to manipulate metrics.
class QUICKEN_EXPORT QuickenMetricsUtils
{
public:
    QuickenMetricsUtils();
    ~QuickenMetricsUtils();

    // Fill the given metrics with updated process metrics.
    void updateProcessMetrics(QuickenMetrics* metrics);

    // Get a time stamp in nanoseconds. The timer is started at the first call,
    // returning 0.
    static quint64 timeStamp();

private:
    QuickenMetricsUtilsPrivate* const d_ptr;
    Q_DECLARE_PRIVATE(QuickenMetricsUtils)
};

#endif  // METRICS_H
