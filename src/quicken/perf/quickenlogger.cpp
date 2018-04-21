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

#include "quickenlogger_p.h"

#include <QtCore/QDir>
#include <QtCore/QTime>

#include "quickenmetrics.h"
#include "quickenglobal_p.h"

QuickenFileLogger::QuickenFileLogger(const QString& fileName, bool parsable)
    : d_ptr(new QuickenFileLoggerPrivate(fileName, parsable))
{
}

QuickenFileLoggerPrivate::QuickenFileLoggerPrivate(const QString& fileName, bool parsable)
{
    if (QDir::isRelativePath(fileName)) {
        m_file.setFileName(QString(QDir::currentPath() + QDir::separator() + fileName));
    } else {
        m_file.setFileName(fileName);
    }

    if (m_file.open(QIODevice::WriteOnly | QIODevice::Text | QIODevice::Unbuffered)) {
        m_textStream.setDevice(&m_file);
        m_textStream.setCodec("ISO 8859-1");
        m_textStream.setRealNumberPrecision(2);
        m_textStream.setRealNumberNotation(QTextStream::FixedNotation);
        m_flags = Open | Parsable;
        if (parsable) {
            m_flags |= Parsable;
        }
    } else {
        m_flags = 0;
        WARN("FileLogger: Can't open file %s '%s'.", fileName.toLatin1().constData(),
             m_file.errorString().toLatin1().constData());
    }
}

QuickenFileLogger::QuickenFileLogger(FILE* fileHandle, bool parsable)
    : d_ptr(new QuickenFileLoggerPrivate(fileHandle, parsable))
{
}

QuickenFileLoggerPrivate::QuickenFileLoggerPrivate(FILE* fileHandle, bool parsable)
{
    if (m_file.open(fileHandle, QIODevice::WriteOnly | QIODevice::Text | QIODevice::Unbuffered)) {
        m_textStream.setDevice(&m_file);
        m_textStream.setCodec("ISO 8859-1");
        m_textStream.setRealNumberPrecision(2);
        m_textStream.setRealNumberNotation(QTextStream::FixedNotation);
        if ((fileHandle == stdout || fileHandle == stderr) &&
            !qEnvironmentVariableIsSet("QUICKEN_NO_LOGGER_COLOR")) {
            m_flags = Open | Colored;
        } else {
            m_flags = Open;
        }
        if (parsable) {
            m_flags |= Parsable;
        }
    } else {
        m_flags = 0;
        WARN("FileLogger: Can't open file handle '%s'.",
             m_file.errorString().toLatin1().constData());
    }
}

QuickenFileLogger::~QuickenFileLogger()
{
    delete d_ptr;
}

bool QuickenFileLogger::isOpen()
{
    return !!(d_func()->m_flags & QuickenFileLoggerPrivate::Open);
}

// FIXME(loicm) We should maybe get rid of QTextStream and directly write to the
//     device for efficiency reasons.

void QuickenFileLogger::log(const QuickenMetrics& metrics)
{
    d_func()->log(metrics);
}

void QuickenFileLoggerPrivate::log(const QuickenMetrics& metrics)
{
    if (m_flags & Open) {
        // ANSI/VT100 terminal codes.
        const char* const dim = m_flags & Colored ? "\033[02m" : "";
        const char* const reset = m_flags & Colored ? "\033[00m" : "";
        const char* const dimColon = m_flags & Colored ? "\033[02m:\033[00m" : "=";

        QTime timeStamp = QTime(0, 0).addMSecs(metrics.timeStamp / 1000000);
        QString timeString = !timeStamp.hour()
            ? timeStamp.toString(QStringLiteral("mm:ss:zzz"))
            : timeStamp.toString(QStringLiteral("hh:mm:ss:zzz"));

        switch (metrics.type) {
        case QuickenMetrics::Process: {
            if (m_flags & Parsable) {
                m_textStream
                    << "P "
                    << metrics.timeStamp << ' '
                    << metrics.process.cpuUsage << ' '
                    << metrics.process.vszMemory << ' '
                    << metrics.process.rssMemory << ' '
                    << metrics.process.threadCount << '\n' << flush;
            } else {
                m_textStream
                    << (m_flags & Colored ? "\033[33mP\033[00m " : "P ")
                    << dim << timeString << reset << ' '
                    << "CPU" << dimColon << metrics.process.cpuUsage << "% "
                    << "VSZ" << dimColon << metrics.process.vszMemory << "kB "
                    << "RSS" << dimColon << metrics.process.rssMemory << "kB "
                    << "Threads" << dimColon << metrics.process.threadCount
                    << '\n' << flush;
            }
            break;
        }

        case QuickenMetrics::Frame:
            if (m_flags & Parsable) {
                m_textStream
                    << "F "
                    << metrics.timeStamp << ' '
                    << metrics.frame.window << ' '
                    << metrics.frame.number << ' '
                    << metrics.frame.deltaTime << ' '
                    << metrics.frame.syncTime << ' '
                    << metrics.frame.renderTime << ' '
                    << metrics.frame.gpuTime << ' '
                    << metrics.frame.swapTime << '\n' << flush;
            } else {
                m_textStream
                    << (m_flags & Colored ? "\033[36mF\033[00m " : "F ")
                    << dim << timeString << reset << ' '
                    << "Win" << dimColon << metrics.frame.window << ' '
                    << "N" << dimColon << metrics.frame.number << ' '
                    << "Delta" << dimColon << metrics.frame.deltaTime / 1000000.0f << "ms "
                    << "Sync" << dimColon << metrics.frame.syncTime / 1000000.0f << "ms "
                    << "Render" << dimColon << metrics.frame.renderTime / 1000000.0f << "ms "
                    << "GPU" << dimColon << metrics.frame.gpuTime / 1000000.0f << "ms "
                    << "Swap" << dimColon << metrics.frame.swapTime / 1000000.0f << "ms\n" << flush;
            }
            break;

        case QuickenMetrics::Window: {
            if (m_flags & Parsable) {
                m_textStream
                    << "W "
                    << metrics.timeStamp << ' '
                    << metrics.window.id << ' '
                    << metrics.window.state << ' '
                    << metrics.window.width << ' '
                    << metrics.window.height << '\n' << flush;
            } else {
                const char* const stateString[] = { "Hidden", "Shown", "Resized" };
                Q_STATIC_ASSERT(ARRAY_SIZE(stateString) == QuickenWindowMetrics::StateCount);
                m_textStream
                    << (m_flags & Colored ? "\033[35mW\033[00m " : "W ")
                    << dim << timeString << reset << ' '
                    << "Id" << dimColon << metrics.window.id << ' '
                    << "State" << dimColon << stateString[metrics.window.state] << ' '
                    << "Size" << dimColon << metrics.window.width << 'x' << metrics.window.height
                    << '\n' << flush;
            }
            break;
        }

        case QuickenMetrics::Generic: {
            if (m_flags & Parsable) {
                m_textStream
                    << "G "
                    << metrics.timeStamp << ' '
                    << metrics.generic.id << ' '
                    << metrics.generic.string << '\n' << flush;
            } else {
                m_textStream
                    << (m_flags & Colored ? "\033[32mG\033[00m " : "G ")
                    << dim << timeString << reset << ' '
                    << "Id" << dimColon << metrics.generic.id << ' '
                    << "String" << dimColon << '"' << metrics.generic.string << '"'
                    << '\n' << flush;
            }
            break;
        }

        default:
            DNOT_REACHED();
            break;
        }
    }
}

void QuickenFileLogger::setParsable(bool parsable)
{
    Q_D(QuickenFileLogger);

    if (parsable) {
        d->m_flags |= QuickenFileLoggerPrivate::Parsable;
    } else {
        d->m_flags &= ~QuickenFileLoggerPrivate::Parsable;
    }
}

bool QuickenFileLogger::parsable()
{
    return !!(d_func()->m_flags & QuickenFileLoggerPrivate::Parsable);
}
