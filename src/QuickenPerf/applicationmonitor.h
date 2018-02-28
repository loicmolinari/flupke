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

#ifndef APPLICATIONMONITOR_H
#define APPLICATIONMONITOR_H

#include <QtCore/QList>

#include <QuickenPerf/logger.h>
#include <QuickenPerf/metrics.h>
#include <QuickenPerf/quickenperfglobal.h>

class QPApplicationMonitorPrivate;

// Monitor a QtQuick application by automatically tracking QtQuick windows and
// process metrics. The metrics gathered can be logged and displayed by an
// overlay rendered on top of each frame.
class QUICKEN_PERF_EXPORT QPApplicationMonitor : public QObject
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
        AllMetrics    = (ProcessMetrics | WindowMetrics | FrameMetrics | GenericMetrics)
    };
    Q_DECLARE_FLAGS(LoggingFilters, LoggingFilter)

    // Get the unique QPApplicationMonitor instance. A QGuiApplication instance
    // must be running.
    static QPApplicationMonitor* instance() { return self ? self : new QPApplicationMonitor; }

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
    QList<QPLogger*> loggers();
    bool installLogger(QPLogger* logger);
    bool removeLogger(QPLogger* logger, bool free = true);
    void clearLoggers(bool free = true);

    // Generic system allowing to log application specific
    // metrics. registerGenericMetrics() returns a unique integer id to be used
    // as first argument to logGenericMetrics(). logGenericMetrics() logs a
    // generic metrics with a dedicated id, a null-terminated string describing
    // the metrics and the string size with the null-terminating character. The
    // maximum string size (with the null-terminating character) is defined in
    // QPGenericMetrics::maxStringSize. Does not log and returns false if
    // logging is disabled or if the logging filter does not contain
    // GenericMetrics.
    quint32 registerGenericMetrics();
    bool logGenericMetrics(quint32 id, const char* string, quint32 size);

    // Set the time in milliseconds between two updates of metrics of a given
    // type. -1 to disable updates. Only QPMetrics::Process is accepted so far
    // as metrics type, default value is 1000. Note that when the overlay is
    // enabled, a process update triggers a frame update.
    void setUpdateInterval(QPMetrics::Type type, int interval);
    int updateInterval(QPMetrics::Type type);

Q_SIGNALS:
    void overlayChanged();
    void loggingChanged();
    void loggingFilterChanged();
    void loggersChanged();
    void updateIntervalChanged(QPMetrics::Type type);

private Q_SLOTS:
    void closeDown();
    void processTimeout();

private:
    static QPApplicationMonitor* self;

    QPApplicationMonitor();
    ~QPApplicationMonitor();
    Q_DISABLE_COPY(QPApplicationMonitor)

    bool eventFilter(QObject* object, QEvent* event) Q_DECL_OVERRIDE;

    QPApplicationMonitorPrivate* const d_ptr;
    Q_DECLARE_PRIVATE(QPApplicationMonitor)
};

#endif  // APPLICATIONMONITOR_H
