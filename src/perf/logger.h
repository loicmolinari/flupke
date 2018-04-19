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

#include <QuickenPerf/quickenperfglobal.h>

class QPFileLoggerPrivate;
struct QPMetrics;

// Log metrics to a specific device.
class QUICKEN_PERF_EXPORT QPLogger
{
public:
    virtual ~QPLogger() {}

    // Log metrics.
    virtual void log(const QPMetrics& metrics) = 0;

    // Get whether the target device has been opened successfully or not.
    virtual bool isOpen() = 0;
};

// Log metrics to a file.
class QUICKEN_PERF_EXPORT QPFileLogger : public QPLogger
{
public:
    QPFileLogger(const QString& filename, bool parsable = true);
    QPFileLogger(FILE* fileHandle, bool parsable = false);
    ~QPFileLogger();

    void log(const QPMetrics& metrics) Q_DECL_OVERRIDE;
    bool isOpen() Q_DECL_OVERRIDE;

    void setParsable(bool parsable);
    bool parsable();

private:
    QPFileLoggerPrivate* const d_ptr;
    Q_DECLARE_PRIVATE(QPFileLogger)
};

#endif  // LOGGER_H
