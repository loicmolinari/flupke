// Copyright © 2017-2018 Loïc Molinari <loicm@loicm.fr>
// Copyright © 2016 Canonical Ltd.
//
// This file is part of Quicken, licensed under the MIT license. See the license
// file at project root for full information.

#ifndef LOGGER_P_H
#define LOGGER_P_H

#include <Quicken/quickenlogger.h>

#include <QtCore/QFile>
#include <QtCore/QTextStream>

#include <Quicken/quickenmetrics.h>
#include <Quicken/private/quickenglobal_p.h>

class QUICKEN_PRIVATE_EXPORT QuickenFileLoggerPrivate
{
public:
    enum {
        Open     = (1 << 0),
        Colored  = (1 << 1),
        Parsable = (1 << 2)
    };

    QuickenFileLoggerPrivate(const QString& fileName, bool parsable);
    QuickenFileLoggerPrivate(FILE* fileHandle, bool parsable);

    void log(const QuickenMetrics& metrics);

    QFile m_file;
    QTextStream m_textStream;
    quint8 m_flags;
};

#endif  // LOGGER_P_H
