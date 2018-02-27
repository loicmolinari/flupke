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

#ifndef APPLICATIONMONITOR_P_H
#define APPLICATIONMONITOR_P_H

#include <QuickenMetrics/applicationmonitor.h>

#include <QtCore/QTimer>
#include <QtCore/QThread>
#include <QtCore/QMutex>
#include <QtCore/QWaitCondition>
#include <QtCore/QElapsedTimer>
#include <QtCore/QRunnable>
#include <QtCore/QAtomicInteger>

#include <QuickenMetrics/private/overlay_p.h>
#include <QuickenMetrics/private/gputimer_p.h>
#include <QuickenMetrics/private/quickenmetricsglobal_p.h>

class LoggingThread;
class WindowMonitor;
class QQuickWindow;

class QUICKEN_METRICS_PRIVATE_EXPORT QMApplicationMonitorPrivate
{
public:
    static const int maxMonitors = 16;
    static const int maxLoggers = 8;

    static inline QMApplicationMonitorPrivate* get(QMApplicationMonitor* applicationMonitor) {
        return applicationMonitor->d_func();
    }

    enum {
        // Lower bit allowed is (1 << 8).
        Overlay     = (1 << 8),
        Logging     = (1 << 9),
        Started     = (1 << 10),
        ClosingDown = (1 << 11),
        // Higher bit allowed is (1 << 15).
        FilterMask             = 0x000000ff,
        ApplicationMonitorMask = 0x0000ff00,
        WindowMonitorMask      = 0xffff0000
    };

    QMApplicationMonitorPrivate(QMApplicationMonitor* applicationMonitor);
    ~QMApplicationMonitorPrivate();

    void startMonitoring(QQuickWindow* window);
    void start();
    bool removeMonitor(WindowMonitor* monitor);
    void stopMonitoring(WindowMonitor* monitor);
    void stop();
    bool hasMonitor(WindowMonitor* monitor);
    void setMonitoringFlags(quint32 flags);
    void processTimeout();

    QMApplicationMonitor* const q_ptr;
    Q_DECLARE_PUBLIC(QMApplicationMonitor)

    WindowMonitor* m_monitors[maxMonitors];
    QMLogger* m_loggers[maxLoggers];
    LoggingThread* m_loggingThread;
#if !defined(QT_NO_DEBUG)
    QGuiApplication* m_application;
#endif
    QMEventUtils m_eventUtils;
    QTimer m_processTimer;
    QMutex m_monitorsMutex;
    int m_monitorCount;
    int m_loggerCount;
    int m_updateInterval[QMEvent::TypeCount];
    quint32 m_flags;
    alignas(64) QMEvent m_processEvent;
};

class QUICKEN_METRICS_PRIVATE_EXPORT LoggingThread : public QThread
{
public:
    LoggingThread();

    void run() override;
    void push(const QMEvent* event);
    void setLoggers(QMLogger** loggers, int count);
    LoggingThread* ref();
    void deref();

private:
    enum {
        Waiting       = (1 << 0),
        JoinRequested = (1 << 1)
    };

    ~LoggingThread();

    QMEvent* m_queue;
    QMLogger* m_loggers[QMApplicationMonitorPrivate::maxLoggers];
    int m_loggerCount;
    QMutex m_mutex;
    QWaitCondition m_condition;
    QAtomicInteger<quint32> m_refCount;
    qint8 m_queueIndex;
    qint8 m_queueSize;
    quint8 m_flags;
};

class QUICKEN_METRICS_PRIVATE_EXPORT WindowMonitorDeleter : public QRunnable
{
public:
    WindowMonitorDeleter(QMApplicationMonitor* applicationMonitor, WindowMonitor* monitor)
        : m_applicationMonitor(applicationMonitor)
        , m_monitor(monitor) {
        DASSERT(applicationMonitor == QMApplicationMonitor::instance());
        DASSERT(m_applicationMonitor);
        DASSERT(monitor);
    }
    ~WindowMonitorDeleter();

    void run() override;

private:
    QMApplicationMonitor* m_applicationMonitor;
    WindowMonitor* m_monitor;
};

class QUICKEN_METRICS_PRIVATE_EXPORT WindowMonitorFlagSetter : public QRunnable
{
public:
    WindowMonitorFlagSetter(WindowMonitor* monitor, quint32 flags)
        : m_applicationMonitor(QMApplicationMonitor::instance())
        , m_monitor(monitor)
        , m_flags(flags) {
        DASSERT(m_applicationMonitor);
        DASSERT(monitor);
    }
    ~WindowMonitorFlagSetter();

    void run() override {}

private:
    QMApplicationMonitor* m_applicationMonitor;
    WindowMonitor* m_monitor;
    quint32 m_flags;
};

class QUICKEN_METRICS_PRIVATE_EXPORT WindowMonitor : public QObject
{
    Q_OBJECT

public:
    WindowMonitor(
        QMApplicationMonitor* applicationMonitor, QQuickWindow* window,
        LoggingThread* loggingThread, quint32 flags, quint32 id);
    ~WindowMonitor();

    QQuickWindow* window() const { return m_window; }
    void setProcessEvent(const QMEvent& event);

private Q_SLOTS:
    void windowSceneGraphInitialized();
    void windowSceneGraphInvalidated();
    void windowBeforeSynchronizing();
    void windowAfterSynchronizing();
    void windowBeforeRendering();
    void windowAfterRendering();
    void windowFrameSwapped();
    void windowSceneGraphAboutToStop();

private:
    enum {
        // Lower bit allowed is (1 << 16).
        GpuResourcesInitialized = (1 << 16),
        GpuTimerAvailable       = (1 << 17),
        SizeChanged             = (1 << 18)
        // Higher bit allowed is (1 << 31).
    };

    bool gpuResourcesInitialized() const { return m_flags & GpuResourcesInitialized; }
    void setFlags(quint32 flags) {
        m_flags = (m_flags & QMApplicationMonitorPrivate::WindowMonitorMask) | flags;
    }
    void initializeGpuResources();
    void finalizeGpuResources();

    QMApplicationMonitor* m_applicationMonitor;
    LoggingThread* m_loggingThread;
    QQuickWindow* m_window;
    QMGPUTimer m_gpuTimer;
    QMOverlay m_overlay;  // Accessed from different threads (needs locking).
    QMutex m_mutex;
    QElapsedTimer m_sceneGraphTimer;
    QElapsedTimer m_deltaTimer;
    quint32 m_id;
    quint32 m_flags;
    QSize m_frameSize;
    QMEvent m_frameEvent;

    friend class WindowMonitorDeleter;
    friend class WindowMonitorFlagSetter;
};

#endif  // APPLICATIONMONITOR_P_H
