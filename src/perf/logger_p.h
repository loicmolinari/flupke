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

#ifndef LOGGER_P_H
#define LOGGER_P_H

#include <QuickenPerf/logger.h>

#include <QtCore/QFile>
#include <QtCore/QTextStream>

#include <QuickenPerf/metrics.h>
#include <QuickenPerf/private/quickenperfglobal_p.h>

class QUICKEN_PERF_PRIVATE_EXPORT QPFileLoggerPrivate
{
public:
    enum {
        Open     = (1 << 0),
        Colored  = (1 << 1),
        Parsable = (1 << 2)
    };

    QPFileLoggerPrivate(const QString& fileName, bool parsable);
    QPFileLoggerPrivate(FILE* fileHandle, bool parsable);

    void log(const QPMetrics& metrics);

    QFile m_file;
    QTextStream m_textStream;
    quint8 m_flags;
};

#endif  // LOGGER_P_H
