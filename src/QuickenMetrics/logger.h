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

#ifndef LOGGER_H
#define LOGGER_H

#include <QtCore/QFile>

#include <QuickenMetrics/quickenmetricsglobal.h>

class QMFileLoggerPrivate;
struct QMEvent;

// Log events to a specific device.
class QUICKEN_METRICS_EXPORT QMLogger
{
public:
    virtual ~QMLogger() {}

    // Log events.
    virtual void log(const QMEvent& event) = 0;

    // Get whether the target device has been opened successfully or not.
    virtual bool isOpen() = 0;
};

// Log events to a file.
class QUICKEN_METRICS_EXPORT QMFileLogger : public QMLogger
{
public:
    QMFileLogger(const QString& filename, bool parsable = true);
    QMFileLogger(FILE* fileHandle, bool parsable = false);
    ~QMFileLogger();

    void log(const QMEvent& event) Q_DECL_OVERRIDE;
    bool isOpen() Q_DECL_OVERRIDE;

    void setParsable(bool parsable);
    bool parsable();

private:
    QMFileLoggerPrivate* const d_ptr;
    Q_DECLARE_PRIVATE(QMFileLogger)
};

#endif  // LOGGER_H
