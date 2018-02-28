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

#include "applicationmonitor_p.h"

#include <QtCore/QTimer>
#include <QtGui/QGuiApplication>
#include <QtQuick/QQuickWindow>

// FIXME(loicm) When a monitored window is destroyed and if there's a window
//     that's not monitored because the max count was reached, enable monitoring
//     on it if possible.

const int logQueueSize = 16;
const int logQueueAlignment = 64;

LoggingThread::LoggingThread()
    : m_loggerCount(0)
    , m_refCount(1)
    , m_queueIndex(0)
    , m_queueSize(0)
    , m_flags(0)
{
    m_queue = static_cast<QMEvent*>(
        alignedAlloc(logQueueAlignment, logQueueSize * sizeof(QMEvent)));

#if !defined(QT_NO_DEBUG)
    setObjectName(QStringLiteral("QuickenMetrics logging"));  // Thread name.
#endif
    start();
}

LoggingThread::~LoggingThread()
{
    m_mutex.lock();
    m_flags |= JoinRequested;
    if (m_flags & Waiting) {
        m_condition.wakeOne();
    }
    m_mutex.unlock();
    wait();

    free(m_queue);
}

#define BREAK_ON_JOIN_REQUEST()                \
    if (Q_UNLIKELY(m_flags & JoinRequested)) { \
        m_mutex.unlock();                      \
        break;                                 \
    }

// Logging thread entry point.
void LoggingThread::run()
{
    DLOG("Entering logging thread.");
    while (true) {
        // Wait for new events in the log queue.
        m_mutex.lock();
        DASSERT(m_queueSize >= 0);
        if (m_queueSize == 0) {
            BREAK_ON_JOIN_REQUEST();
            m_flags |= Waiting;
            m_condition.wait(&m_mutex);
            if (m_queueSize == 0) {
                BREAK_ON_JOIN_REQUEST();
            }
            m_flags &= ~Waiting;
        }

        // Unqueue oldest events from the log queue.
        DASSERT(m_queueSize > 0);
        QMEvent event;
        memcpy(&event, &m_queue[m_queueIndex], sizeof(QMEvent));
        m_queueIndex = (m_queueIndex + 1) % logQueueSize;
        m_queueSize--;

        // Log.
        const int loggerCount = m_loggerCount;
        QMLogger* loggers[QMApplicationMonitorPrivate::maxLoggers];
        memcpy(loggers, m_loggers, loggerCount * sizeof(QMLogger*));
        m_mutex.unlock();
        for (int i = 0; i < loggerCount; ++i) {
            loggers[i]->log(event);
        }
    }
    DLOG("Leaving logging thread.");
}

void LoggingThread::push(const QMEvent* event)
{
    // Ensure the log queue is not full.
    m_mutex.lock();
    DASSERT(m_queueSize <= logQueueSize);
    while (m_queueSize == logQueueSize) {
        m_mutex.unlock();
        QThread::yieldCurrentThread();
        m_mutex.lock();
    }

    // Push event to the log queue.
    DASSERT(m_queueSize < logQueueSize);
    memcpy(&m_queue[(m_queueIndex + m_queueSize++) % logQueueSize], event, sizeof(QMEvent));
    if (m_flags & Waiting) {
        m_condition.wakeOne();
    }
    m_mutex.unlock();
}

void LoggingThread::setLoggers(QMLogger** loggers, int count)
{
    DASSERT(count >= 0);
    DASSERT(count <= QMApplicationMonitorPrivate::maxLoggers);

    QMutexLocker locker(&m_mutex);
    memcpy(m_loggers, loggers, count * sizeof(QMLogger*));
    m_loggerCount = count;
}

LoggingThread* LoggingThread::ref()
{
    m_refCount.ref();
    return this;
}

void LoggingThread::deref()
{
    if (m_refCount.deref() == 0) {
        if (thread() == QThread::currentThread()) {
            delete this;
        } else {
            deleteLater();
        }
    }
}

QMApplicationMonitor* QMApplicationMonitor::self = nullptr;

QMApplicationMonitor::QMApplicationMonitor()
    : d_ptr(new QMApplicationMonitorPrivate(this))
{
    ASSERT_X(!self, "ApplicationMonitor: There should be only one QMApplicationMonitor.");
    self = this;
}

QMApplicationMonitorPrivate::QMApplicationMonitorPrivate(QMApplicationMonitor* applicationMonitor)
    : q_ptr(applicationMonitor)
#if !defined(QT_NO_DEBUG)
    , m_monitors{}
    , m_loggers{}
#endif
    , m_loggingThread(nullptr)
    , m_monitorCount(0)
    , m_loggerCount(0)
    , m_updateInterval{1000, -1, -1}
    , m_flags(QMApplicationMonitor::AllEvents)
{
    Q_Q(QMApplicationMonitor);

    QGuiApplication* application = qobject_cast<QGuiApplication*>(QCoreApplication::instance());
    ASSERT_X(application, "ApplicationMonitor: There should be a QGuiApplication instantiated.");
#if !defined(QT_NO_DEBUG)
    m_application = application;
#endif

    q->setParent(application);

    QObject::connect(application, SIGNAL(lastWindowClosed()), q, SLOT(closeDown()));
    QObject::connect(application, SIGNAL(aboutToQuit()), q, SLOT(closeDown()));
    QObject::connect(&m_processTimer, SIGNAL(timeout()), q, SLOT(processTimeout()));

    m_processTimer.setInterval(m_updateInterval[QMEvent::Process]);
}

QMApplicationMonitor::~QMApplicationMonitor()
{
    delete d_ptr;
}

QMApplicationMonitorPrivate::~QMApplicationMonitorPrivate()
{
    DASSERT(!(m_flags & Started));

    // Note that there's no need to disconnect from QGuiApplication signals
    // since the application monitor instance is automatically destroyed when
    // the application is destroyed (parenting), the application instance would
    // be NULL.
}

void QMApplicationMonitor::setOverlay(bool overlay)
{
    Q_D(QMApplicationMonitor);

    if (!!(d->m_flags & QMApplicationMonitorPrivate::Overlay) != overlay) {
        if (overlay) {
            d->m_flags |= QMApplicationMonitorPrivate::Overlay;
            if (!(d->m_flags & (QMApplicationMonitorPrivate::Started
                                | QMApplicationMonitorPrivate::ClosingDown))) {
                d->start();
            } else {
                d->setMonitoringFlags(d->m_flags);
            }
        } else {
            d->m_flags &= ~QMApplicationMonitorPrivate::Overlay;
            if (!(d->m_flags & QMApplicationMonitorPrivate::Logging)) {
                d->stop();
            } else {
                d->setMonitoringFlags(d->m_flags);
            }
        }
        Q_EMIT overlayChanged();
    }
}

bool QMApplicationMonitor::overlay()
{
    return !!(d_func()->m_flags & QMApplicationMonitorPrivate::Overlay);
}

void QMApplicationMonitor::setLogging(bool logging)
{
    Q_D(QMApplicationMonitor);

    if (!!(d->m_flags & QMApplicationMonitorPrivate::Logging) != logging) {
        if (logging) {
            d->m_flags |= QMApplicationMonitorPrivate::Logging;
            if (!(d->m_flags & (QMApplicationMonitorPrivate::Started
                                | QMApplicationMonitorPrivate::ClosingDown))) {
                d->start();
            } else {
                d->setMonitoringFlags(d->m_flags);
            }
        } else {
            d->m_flags &= ~QMApplicationMonitorPrivate::Logging;
            if (!(d->m_flags & QMApplicationMonitorPrivate::Overlay)) {
                d->stop();
            } else {
                d->setMonitoringFlags(d->m_flags);
            }
        }
        Q_EMIT loggingChanged();
    }
}

bool QMApplicationMonitor::logging()
{
    return !!(d_func()->m_flags & QMApplicationMonitorPrivate::Logging);
}

void QMApplicationMonitorPrivate::startMonitoring(QQuickWindow* window)
{
    DASSERT(window);
    DASSERT(m_loggingThread);

    if (m_monitorCount < maxMonitors) {
        DASSERT(m_monitors[m_monitorCount] == nullptr);
        static quint32 id = 0;
        m_monitors[m_monitorCount] =
            new WindowMonitor(q_func(), window, m_loggingThread->ref(), m_flags, ++id);
        m_monitors[m_monitorCount]->setProcessEvent(m_processEvent);
        m_monitorCount++;
    } else {
        WARN("ApplicationMonitor: Can't monitor more than %d QQuickWindows.", maxMonitors);
    }
}

void QMApplicationMonitorPrivate::start()
{
    DASSERT(!(m_flags & Started));
    DASSERT(!m_loggingThread);

    m_loggingThread = new LoggingThread;
    m_loggingThread->setLoggers(m_loggers, m_loggerCount);

    QWindowList windows = QGuiApplication::allWindows();
    const int size = windows.size();
    m_monitorsMutex.lock();
    for (int i = 0; i < size; ++i) {
        if (QQuickWindow* window = qobject_cast<QQuickWindow*>(windows[i])) {
            if (window->isVisible()) {
                startMonitoring(window);
            }
        }
    }
    m_monitorsMutex.unlock();

    QGuiApplication::instance()->installEventFilter(q_func());

    // Doing it here so that processTimeout can assert the monitoring started.
    m_flags |= Started;

    memset(&m_processEvent, 0, sizeof(QMEvent));
    processTimeout();
    if (m_updateInterval[QMEvent::Process] >= 0) {
        m_processTimer.start();
    }
}

bool QMApplicationMonitorPrivate::removeMonitor(WindowMonitor* monitor)
{
    DASSERT(monitor);
    DASSERT(monitor->window());

    m_monitorsMutex.lock();
    for (int i = 0; i < m_monitorCount; ++i) {
        if (m_monitors[i] == monitor) {
            m_monitorCount--;
            if (i < m_monitorCount) {
                memmove(&m_monitors[i], &m_monitors[i+1],
                        (m_monitorCount-i) * sizeof(WindowMonitor*));
            }
#if !defined(QT_NO_DEBUG)
            m_monitors[m_monitorCount] = nullptr;
#endif
            m_monitorsMutex.unlock();
            
            return true;
        }
    }
    m_monitorsMutex.unlock();

    return false;
}

WindowMonitorDeleter::~WindowMonitorDeleter()
{
    // FIXME(loicm) Not sure if that assertion could ever fail, so let's debug
    //     assert it and test in release for now.
    DASSERT(m_applicationMonitor == QMApplicationMonitor::instance());
    if (m_applicationMonitor == QMApplicationMonitor::instance()) {
        // Remove the monitor from the list making sure it's not been removed
        // (window going hidden) after this runnable was scheduled.
        if (QMApplicationMonitorPrivate::get(m_applicationMonitor)->removeMonitor(m_monitor)) {
            delete m_monitor;
        }
    }
}

void WindowMonitorDeleter::run()
{
    // run() guarantees a valid context.
    if (m_monitor->gpuResourcesInitialized()) {
        m_monitor->finalizeGpuResources();
    }
}

void QMApplicationMonitorPrivate::stopMonitoring(WindowMonitor* monitor)
{
    DASSERT(monitor);
    DASSERT(monitor->window());

    // To ensure there's no signals emitted on the render thread while and after
    // window monitor deletion, we schedule a render job that disconnects the
    // signals on the render thread and then request deletion on the right
    // thread with deleteLater().
    monitor->window()->scheduleRenderJob(
        new WindowMonitorDeleter(q_func(), monitor), QQuickWindow::NoStage);
}

void QMApplicationMonitorPrivate::stop()
{
    DASSERT(m_flags & Started);

    if (m_updateInterval[QMEvent::Process] >= 0) {
        m_processTimer.stop();
    }

    QGuiApplication::instance()->removeEventFilter(q_func());

    // scheduleRenderJobs() could possibly execute jobs right now we must loop
    // over a copy to avoid deadlocks.
    WindowMonitor* monitorsCopy[maxMonitors];
    m_monitorsMutex.lock();
    int monitorCountCopy = m_monitorCount;
    memcpy(monitorsCopy, m_monitors, m_monitorCount * sizeof(WindowMonitor*));
    m_monitorsMutex.unlock();
    for (int i = 0; i < monitorCountCopy; ++i) {
        stopMonitoring(monitorsCopy[i]);
    }

    DASSERT(m_loggingThread);
    m_loggingThread->deref();
    m_loggingThread = nullptr;

    // Wait for window monitors complete deletion.
    m_monitorsMutex.lock();
    while (m_monitorCount > 0) {
        // FIXME(loicm) Should we yield the thread here in the case of a
        //     threaded QtQuick renderer?
        m_monitorsMutex.unlock();
        QCoreApplication::processEvents(QEventLoop::ExcludeUserInputEvents);
        m_monitorsMutex.lock();
    }
    m_monitorsMutex.unlock();

    m_flags &= ~Started;
}

bool QMApplicationMonitorPrivate::hasMonitor(WindowMonitor* monitor)
{
    DASSERT(monitor);
    DASSERT(monitor->window());

    m_monitorsMutex.lock();
    for (int i = 0; i < m_monitorCount; ++i) {
        if (m_monitors[i] == monitor) {
            m_monitorsMutex.unlock();
            return true;
        }
    }
    m_monitorsMutex.unlock();
    return false;
}

WindowMonitorFlagSetter::~WindowMonitorFlagSetter()
{
    // FIXME(loicm) Not sure if that assertion could ever fail, so let's debug
    //     assert it and do the check in release for now.
    DASSERT(m_applicationMonitor == QMApplicationMonitor::instance());
    if (m_applicationMonitor == QMApplicationMonitor::instance()) {
        // Make sure it's not been removed (window going hidden) after this
        // runnable was scheduled.
        if (QMApplicationMonitorPrivate::get(m_applicationMonitor)->hasMonitor(m_monitor)) {
            m_monitor->setFlags(m_flags);
        }
    }
}

void QMApplicationMonitorPrivate::setMonitoringFlags(quint32 flags)
{
    // scheduleRenderJobs() could possibly execute jobs right now we must loop
    // over a copy to avoid deadlocks.
    WindowMonitor* monitorsCopy[maxMonitors];
    m_monitorsMutex.lock();
    int monitorCountCopy = m_monitorCount;
    memcpy(monitorsCopy, m_monitors, m_monitorCount * sizeof(WindowMonitor*));
    m_monitorsMutex.unlock();
    for (int i = 0; i < monitorCountCopy; ++i) {
        DASSERT(monitorsCopy[i]);
        DASSERT(monitorsCopy[i]->window());
        monitorsCopy[i]->window()->scheduleRenderJob(
            new WindowMonitorFlagSetter(monitorsCopy[i], flags), QQuickWindow::NoStage);
    }
}

void QMApplicationMonitor::setLoggingFilter(QMApplicationMonitor::LoggingFilters filter)
{
    Q_D(QMApplicationMonitor);

    const quint32 maskedFilter = filter & QMApplicationMonitorPrivate::FilterMask;
    if (maskedFilter != (d->m_flags & QMApplicationMonitorPrivate::FilterMask)) {
        d->m_flags = (d->m_flags & ~QMApplicationMonitorPrivate::FilterMask) | maskedFilter;
        if (d->m_flags & QMApplicationMonitorPrivate::Started) {
            d->setMonitoringFlags(d->m_flags);
        }
        Q_EMIT loggingFilterChanged();
    }
}

QMApplicationMonitor::LoggingFilters QMApplicationMonitor::loggingFilter()
{
    return static_cast<LoggingFilters>(d_func()->m_flags & QMApplicationMonitorPrivate::FilterMask);
}

QList<QMLogger*> QMApplicationMonitor::loggers()
{
    Q_D(QMApplicationMonitor);

    QList<QMLogger*> list;
    const int count = d->m_loggerCount;
    for (int i = 0; i < count; ++i) {
        list.append(d->m_loggers[i]);
    }
    return list;
}

bool QMApplicationMonitor::installLogger(QMLogger* logger)
{
    Q_D(QMApplicationMonitor);

    if (d->m_loggerCount < QMApplicationMonitorPrivate::maxLoggers && logger) {
        DASSERT(d->m_loggers[d->m_loggerCount] == nullptr);
        d->m_loggers[d->m_loggerCount++] = logger;
        if (d->m_flags & QMApplicationMonitorPrivate::Started) {
            DASSERT(d->m_loggingThread);
            d->m_loggingThread->setLoggers(d->m_loggers, d->m_loggerCount);
        }
        Q_EMIT loggersChanged();
        return true;
    } else {
        return false;
    }
}

bool QMApplicationMonitor::removeLogger(QMLogger* logger, bool free)
{
    if (!logger) {
        return false;
    }

    Q_D(QMApplicationMonitor);

    for (int i = d->m_loggerCount; i > 0; --i) {
        if (d->m_loggers[i] == logger) {
            if (i < --d->m_loggerCount) {
                d->m_loggers[i] = d->m_loggers[d->m_loggerCount];
            }
#if !defined(QT_NO_DEBUG)
            d->m_loggers[d->m_loggerCount] = nullptr;
#endif
            if (d->m_flags & QMApplicationMonitorPrivate::Started) {
                DASSERT(d->m_loggingThread);
                d->m_loggingThread->setLoggers(d->m_loggers, d->m_loggerCount);
            }
            if (free) {
                delete logger;
            }
            Q_EMIT loggersChanged();
            return true;
        }
    }
    return false;
}

void QMApplicationMonitor::clearLoggers(bool free)
{
    Q_D(QMApplicationMonitor);

    if (d->m_loggerCount > 0) {
        if (free) {
            const int count = d->m_loggerCount;
            for (int i = 0; i < count; ++i) {
                delete d->m_loggers[i];
            }
        }
        d->m_loggerCount = 0;
        Q_EMIT loggersChanged();
    }
}

quint32 QMApplicationMonitor::registerGenericEvent()
{
    static quint32 id = 0;  // 0 is reserved for QMApplicationMonitor events.
    return ++id;
}

bool QMApplicationMonitor::logGenericEvent(quint32 id, const char* string, quint32 size)
{
    Q_D(QMApplicationMonitor);

    if ((d->m_flags & QMApplicationMonitorPrivate::Logging) && (d->m_flags & GenericEvent)) {
        DASSERT(d->m_loggingThread);
        QMEvent event;
        event.type = QMEvent::Generic;
        event.timeStamp = QMEventUtils::timeStamp();
        event.generic.id = id;
        // We don't bother fixing up non null-terminated string, just potential
        // overflows.
        event.generic.stringSize = qMin(size, static_cast<quint32>(QMGenericEvent::maxStringSize));
        memcpy(event.generic.string, string, event.generic.stringSize);
        d->m_loggingThread->push(&event);
        return true;
    } else {
        return false;
    }
}

void QMApplicationMonitor::setUpdateInterval(QMEvent::Type type, int interval)
{
    Q_D(QMApplicationMonitor);

    if (type == QMEvent::Process) {
        // Other types (like QMEvent::Frame) are ignored for now.
        if (interval != d->m_updateInterval[QMEvent::Process]) {
            if (interval >= 0) {
                d->m_processTimer.setInterval(interval);
                if ((d->m_flags & QMApplicationMonitorPrivate::Started)
                    && (d->m_updateInterval[QMEvent::Process] < 0)) {
                    d->m_processTimer.start();
                }
            } else if ((d->m_flags & QMApplicationMonitorPrivate::Started)
                       && (d->m_updateInterval[QMEvent::Process] >= 0)) {
                d->m_processTimer.stop();
            }
            d->m_updateInterval[QMEvent::Process] = interval;
            Q_EMIT updateIntervalChanged(QMEvent::Process);
        }
    }
}

int QMApplicationMonitor::updateInterval(QMEvent::Type type)
{
    return d_func()->m_updateInterval[type];
}

void QMApplicationMonitor::closeDown()
{
    Q_D(QMApplicationMonitor);

    d->m_flags |= QMApplicationMonitorPrivate::ClosingDown;
    if (d->m_flags & QMApplicationMonitorPrivate::Started) {
        d->stop();
    }
}

void QMApplicationMonitor::processTimeout()
{
    d_func()->processTimeout();
}

void QMApplicationMonitorPrivate::processTimeout()
{
    DASSERT(m_flags & Started);
    DASSERT(m_loggingThread);

    const bool processLogging =
        (m_flags & Logging) && (m_flags & QMApplicationMonitor::ProcessEvent);
    const bool overlay = m_flags & Overlay;

    if (processLogging || overlay) {
        m_eventUtils.updateProcessEvent(&m_processEvent);
        if (processLogging) {
            m_loggingThread->push(&m_processEvent);
        }
        if (overlay) {
            // FIXME(loicm) We've got two choices here, locking all the monitors
            //     and pushing the new process event or using scheduleRenderJob.
            //     We're using direct pushing for now but it would be nice to
            //     measure the cost of both methods at high update freqs. The
            //     former one implying locks and the second one dynamic memory
            //     allocations (placement new to the rescue?) and copies, and
            //     also I guess locks at the QtQuick level.
            m_monitorsMutex.lock();
            for (int i = 0; i < m_monitorCount; ++i) {
                DASSERT(m_monitors[i]);
                m_monitors[i]->setProcessEvent(m_processEvent);
            }
            m_monitorsMutex.unlock();
        }
    }
}

bool QMApplicationMonitor::eventFilter(QObject* object, QEvent* event)
{
    if (event->type() == QEvent::Show) {
        if (QQuickWindow* window = qobject_cast<QQuickWindow*>(object)) {
            Q_D(QMApplicationMonitor);
            d->m_monitorsMutex.lock();
            d->startMonitoring(window);
            d->m_monitorsMutex.unlock();
        }
    }
    return QObject::eventFilter(object, event);
}

static const char* const defaultOverlayText =
    "%qtVersion (%qtPlatform) - %glVersion\n"
    "%cpuModel\n"  // FIXME(loicm) Should be included by default?
    "%gpuModel\r"  // FIXME(loicm) Should be included by default?
    "    Window : %9windowId   \n"
    "      Size : %9windowSize px\r"
    "     Frame : %9frameNumber   \n"
    // FIXME(loicm) should be removed once we have a timing histogram with swap included.
    " Delta n-1 : %9deltaTime ms\n"
    "  SG sync. : %9syncTime ms\n"
    " SG render : %9renderTime ms\n"
    "       GPU : %9gpuTime ms\n"
    "     Total : %9totalTime ms\r"
    "  VSZ mem. : %9vszMemory kB\n"
    "  RSS mem. : %9rssMemory kB\n"
    "   Threads : %9threadCount   \n"
    " CPU usage : %9cpuUsage %% ";

WindowMonitor::WindowMonitor(
    QMApplicationMonitor* applicationMonitor, QQuickWindow* window, LoggingThread* loggingThread,
    quint32 flags, quint32 id)
    : m_applicationMonitor(applicationMonitor)
    , m_loggingThread(loggingThread)
    , m_window(window)
    , m_overlay(defaultOverlayText, id)
    , m_id(id)
    , m_flags(flags)
    , m_frameSize(window->width(), window->height())
{
    DASSERT(applicationMonitor == QMApplicationMonitor::instance());
    DASSERT(m_applicationMonitor);
    DASSERT(window);
    DASSERT(loggingThread);

    moveToThread(nullptr);

    QObject::connect(window, SIGNAL(sceneGraphInitialized()), this,
                     SLOT(windowSceneGraphInitialized()), Qt::DirectConnection);
    QObject::connect(window, SIGNAL(sceneGraphInvalidated()), this,
                     SLOT(windowSceneGraphInvalidated()), Qt::DirectConnection);
    QObject::connect(window, SIGNAL(beforeSynchronizing()), this,
                     SLOT(windowBeforeSynchronizing()), Qt::DirectConnection);
    QObject::connect(window, SIGNAL(afterSynchronizing()), this,
                     SLOT(windowAfterSynchronizing()), Qt::DirectConnection);
    QObject::connect(window, SIGNAL(beforeRendering()), this, SLOT(windowBeforeRendering()),
                     Qt::DirectConnection);
    QObject::connect(window, SIGNAL(afterRendering()), this, SLOT(windowAfterRendering()),
                     Qt::DirectConnection);
    QObject::connect(window, SIGNAL(frameSwapped()), this, SLOT(windowFrameSwapped()),
                     Qt::DirectConnection);
    QObject::connect(window, SIGNAL(sceneGraphAboutToStop()), this,
                     SLOT(windowSceneGraphAboutToStop()), Qt::DirectConnection);

    memset(&m_frameEvent, 0, sizeof(m_frameEvent));
    m_frameEvent.type = QMEvent::Frame;
    m_frameEvent.frame.window = id;

    if ((flags & QMApplicationMonitorPrivate::Logging)
        && (flags & QMApplicationMonitor::WindowEvent)) {
        QMEvent event;
        event.type = QMEvent::Window;
        event.timeStamp = QMEventUtils::timeStamp();
        event.window.id = id;
        event.window.width = m_frameSize.width();
        event.window.height = m_frameSize.height();
        event.window.state = QMWindowEvent::Shown;
        loggingThread->push(&event);
    }
}

WindowMonitor::~WindowMonitor()
{
    DASSERT(!(m_flags & GpuResourcesInitialized));

    if ((m_flags & QMApplicationMonitorPrivate::Logging)
        && (m_flags & QMApplicationMonitor::WindowEvent)) {
        QMEvent event;
        event.type = QMEvent::Window;
        event.timeStamp = QMEventUtils::timeStamp();
        event.window.id = m_id;
        event.window.width = m_frameSize.width();
        event.window.height = m_frameSize.height();
        event.window.state = QMWindowEvent::Hidden;
        m_loggingThread->push(&event);
    }

    m_loggingThread->deref();
}

void WindowMonitor::initializeGpuResources()
{
    DASSERT(!(m_flags & GpuResourcesInitialized));

    // FIXME(loicm) We should actually provide an API call to let the user set
    //     that behavior programmatically.
    static bool noGpuTimer = qEnvironmentVariableIsSet("QM_NO_GPU_TIMER");

    m_overlay.initialize();
    m_gpuTimer.initialize();
    m_frameEvent.frame.number = 0;
    m_flags |= GpuResourcesInitialized | (!noGpuTimer ? GpuTimerAvailable : 0);
}

void WindowMonitor::windowSceneGraphInitialized()
{
    if (!(m_flags & GpuResourcesInitialized)) {
        initializeGpuResources();
    }
}

void WindowMonitor::finalizeGpuResources()
{
    DASSERT(m_flags & GpuResourcesInitialized);

    if (m_flags & GpuTimerAvailable) {
        m_gpuTimer.finalize();
    }
    m_overlay.finalize();

    m_frameEvent.frame.number = 0;
    m_flags &= ~(GpuResourcesInitialized | GpuTimerAvailable);
}

void WindowMonitor::windowSceneGraphInvalidated()
{
    if (m_flags & GpuResourcesInitialized) {
        finalizeGpuResources();
    }
}

void WindowMonitor::windowBeforeSynchronizing()
{
    if (m_flags & GpuResourcesInitialized) {
        m_sceneGraphTimer.start();
    }
}

void WindowMonitor::windowAfterSynchronizing()
{
    if (m_flags & GpuResourcesInitialized) {
        m_frameEvent.frame.syncTime = m_sceneGraphTimer.nsecsElapsed();
    }
}

void WindowMonitor::windowBeforeRendering()
{
    const QSize frameSize = m_window->size();
    if (frameSize != m_frameSize) {
        m_frameSize = frameSize;
        if ((m_flags & QMApplicationMonitorPrivate::Logging) &&
            (m_flags & QMApplicationMonitor::WindowEvent)) {
            QMEvent event;
            event.type = QMEvent::Window;
            event.timeStamp = QMEventUtils::timeStamp();
            event.window.id = m_id;
            event.window.width = frameSize.width();
            event.window.height = frameSize.height();
            event.window.state = QMWindowEvent::Resized;
            m_loggingThread->push(&event);
        }
    }

    if (m_flags & GpuResourcesInitialized) {
        m_sceneGraphTimer.start();
        if (m_flags & GpuTimerAvailable) {
            m_gpuTimer.start();
        }
    }
}

void WindowMonitor::windowAfterRendering()
{
    if (m_flags & GpuResourcesInitialized) {
        m_frameEvent.frame.renderTime = m_sceneGraphTimer.nsecsElapsed();
        m_frameEvent.frame.gpuTime = (m_flags & GpuTimerAvailable) ? m_gpuTimer.stop() : 0;
        m_frameEvent.frame.number++;
        if (m_flags & QMApplicationMonitorPrivate::Overlay) {
            m_mutex.lock();
            m_overlay.render(m_frameEvent, m_frameSize);
            m_mutex.unlock();
        }
        m_sceneGraphTimer.start();
    }
}

void WindowMonitor::windowFrameSwapped()
{
    if (m_flags & GpuResourcesInitialized) {
        m_frameEvent.frame.deltaTime = m_deltaTimer.isValid() ? m_deltaTimer.nsecsElapsed() : 0;
        m_deltaTimer.start();
        if ((m_flags & QMApplicationMonitorPrivate::Logging) &&
            (m_flags & QMApplicationMonitor::FrameEvent)) {
            m_frameEvent.frame.swapTime = m_sceneGraphTimer.nsecsElapsed();
            m_frameEvent.timeStamp = QMEventUtils::timeStamp();
            m_loggingThread->push(&m_frameEvent);
        }
    } else {
        initializeGpuResources();  // Get everything ready for the next frame.
        if (m_flags & QMApplicationMonitorPrivate::Overlay) {
            m_window->update();
        }
    }
}

void WindowMonitor::windowSceneGraphAboutToStop()
{
#if !defined(QT_NO_DEBUG)
    ASSERT(m_applicationMonitor == QMApplicationMonitor::instance());
    ASSERT(QMApplicationMonitorPrivate::get(m_applicationMonitor)->removeMonitor(this) == true);
#else
    QMApplicationMonitorPrivate::get(m_applicationMonitor)->removeMonitor(this);
#endif

    if (m_flags & GpuResourcesInitialized) {
        finalizeGpuResources();
    }
    delete this;
}

void WindowMonitor::setProcessEvent(const QMEvent& event)
{
    DASSERT(event.type == QMEvent::Process);

    if (m_flags & QMApplicationMonitorPrivate::Overlay) {
        m_mutex.lock();
        m_overlay.setProcessEvent(event);
        m_mutex.unlock();
        m_window->update();
    }
}
