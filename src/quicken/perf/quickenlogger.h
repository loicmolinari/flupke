// Copyright © 2017-2018 Loïc Molinari <loicm@loicm.fr>
// Copyright © 2016 Canonical Ltd.
//
// This file is part of Quicken, licensed under the MIT license. See the license
// file at project root for full information.

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
