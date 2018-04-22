// Copyright © 2017-2018 Loïc Molinari <loicm@loicm.fr>
// Copyright © 2016 Canonical Ltd.
//
// This file is part of Quicken, licensed under the MIT license. See the license
// file at project root for full information.

#ifndef APPLICATIONMONITOR_H
#define APPLICATIONMONITOR_H

#include <QtCore/QList>

#include <Quicken/quickenlogger.h>
#include <Quicken/quickenmetrics.h>
#include <Quicken/quickenglobal.h>

class QuickenApplicationMonitorPrivate;

// Monitor a QtQuick application by automatically tracking QtQuick windows and
// process metrics. The metrics gathered can be logged and displayed by an
// overlay rendered on top of each frame.
class QUICKEN_EXPORT QuickenApplicationMonitor : public QObject
{
    Q_OBJECT

public:
    enum LoggingFilter {
        // Allow process metrics logging.
        ProcessMetrics = (1 << 0),
        // Allow window metrics logging.
        WindowMetrics  = (1 << 1),
        // Allow frame metrics logging.
        FrameMetrics   = (1 << 2),
        // Allow generic metrics logging.
        GenericMetrics = (1 << 3),
        // Allow all metrics logging.
        AllMetrics     = (ProcessMetrics | WindowMetrics | FrameMetrics | GenericMetrics)
    };
    Q_DECLARE_FLAGS(LoggingFilters, LoggingFilter)

    // Get the unique QuickenApplicationMonitor instance. A QGuiApplication instance
    // must be running.
    static QuickenApplicationMonitor* instance() {
        return self ? self : new QuickenApplicationMonitor;
    }

    // Render an overlay of real-time metrics on top of each QtQuick frame.
    void setOverlay(bool overlay);
    bool overlay();

    // Log the metrics with the installed loggers.
    void setLogging(bool logging);
    bool logging();

    // Set the logging filter. All metrics are logged by default.
    void setLoggingFilter(LoggingFilters filter);
    LoggingFilters loggingFilter();

    // Set the loggers. Empty by default, max number of loggers is 8.
    QList<QuickenLogger*> loggers();
    bool installLogger(QuickenLogger* logger);
    bool removeLogger(QuickenLogger* logger, bool free = true);
    void clearLoggers(bool free = true);

    // Generic system allowing to log application specific
    // metrics. registerGenericMetrics() returns a unique integer id to be used
    // as first argument to logGenericMetrics(). logGenericMetrics() logs a
    // generic metrics with a dedicated id, a null-terminated string describing
    // the metrics and the string size with the null-terminating character. The
    // maximum string size (with the null-terminating character) is defined in
    // QuickenGenericMetrics::maxStringSize. Does not log and returns false if
    // logging is disabled or if the logging filter does not contain
    // GenericMetrics.
    quint32 registerGenericMetrics();
    bool logGenericMetrics(quint32 id, const char* string, quint32 size);

    // Set the time in milliseconds between two updates of metrics of a given
    // type. -1 to disable updates. Only QuickenMetrics::Process is accepted so
    // far as metrics type, default value is 1000. Note that when the overlay is
    // enabled, a process update triggers a frame update.
    void setUpdateInterval(QuickenMetrics::Type type, int interval);
    int updateInterval(QuickenMetrics::Type type);

Q_SIGNALS:
    void overlayChanged();
    void loggingChanged();
    void loggingFilterChanged();
    void loggersChanged();
    void updateIntervalChanged(QuickenMetrics::Type type);

private Q_SLOTS:
    void closeDown();
    void processTimeout();

private:
    static QuickenApplicationMonitor* self;

    QuickenApplicationMonitor();
    ~QuickenApplicationMonitor();
    Q_DISABLE_COPY(QuickenApplicationMonitor)

    bool eventFilter(QObject* object, QEvent* event) Q_DECL_OVERRIDE;

    QuickenApplicationMonitorPrivate* const d_ptr;
    Q_DECLARE_PRIVATE(QuickenApplicationMonitor)
};

#endif  // APPLICATIONMONITOR_H
