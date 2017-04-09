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

#ifndef LOGGER_H
#define LOGGER_H

#include <QtCore/QFile>

#include <FlupkeMetrics/flupkemetricsglobal.h>

class FMFileLoggerPrivate;
struct FMEvent;

// Log events to a specific device.
class FLUPKE_METRICS_EXPORT FMLogger
{
public:
    virtual ~FMLogger() {}

    // Log events.
    virtual void log(const FMEvent& event) = 0;

    // Get whether the target device has been opened successfully or not.
    virtual bool isOpen() = 0;
};

// Log events to a file.
class FLUPKE_METRICS_EXPORT FMFileLogger : public FMLogger
{
public:
    FMFileLogger(const QString& filename, bool parsable = true);
    FMFileLogger(FILE* fileHandle, bool parsable = false);
    ~FMFileLogger();

    void log(const FMEvent& event) Q_DECL_OVERRIDE;
    bool isOpen() Q_DECL_OVERRIDE;

    void setParsable(bool parsable);
    bool parsable();

private:
    FMFileLoggerPrivate* const d_ptr;
    Q_DECLARE_PRIVATE(FMFileLogger)
};

#endif  // LOGGER_H
