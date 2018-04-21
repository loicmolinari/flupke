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

#include <Quicken/quickenglobal.h>

class QuickenFileLoggerPrivate;
struct QuickenMetrics;

// Log metrics to a specific device.
class QUICKEN_EXPORT QuickenLogger
{
public:
    virtual ~QuickenLogger() {}

    // Log metrics.
    virtual void log(const QuickenMetrics& metrics) = 0;

    // Get whether the target device has been opened successfully or not.
    virtual bool isOpen() = 0;
};

// Log metrics to a file.
class QUICKEN_EXPORT QuickenFileLogger : public QuickenLogger
{
public:
    QuickenFileLogger(const QString& filename, bool parsable = true);
    QuickenFileLogger(FILE* fileHandle, bool parsable = false);
    ~QuickenFileLogger();

    void log(const QuickenMetrics& metrics) Q_DECL_OVERRIDE;
    bool isOpen() Q_DECL_OVERRIDE;

    void setParsable(bool parsable);
    bool parsable();

private:
    QuickenFileLoggerPrivate* const d_ptr;
    Q_DECLARE_PRIVATE(QuickenFileLogger)
};

#endif  // LOGGER_H