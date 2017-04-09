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

#ifndef LOGGER_P_H
#define LOGGER_P_H

#include <FlupkeMetrics/logger.h>

#include <QtCore/QFile>
#include <QtCore/QTextStream>

#include <FlupkeMetrics/events.h>
#include <FlupkeMetrics/private/flupkemetricsglobal_p.h>

class FLUPKE_METRICS_PRIVATE_EXPORT FMFileLoggerPrivate
{
public:
    enum {
        Open     = (1 << 0),
        Colored  = (1 << 1),
        Parsable = (1 << 2)
    };

    FMFileLoggerPrivate(const QString& fileName, bool parsable);
    FMFileLoggerPrivate(FILE* fileHandle, bool parsable);

    void log(const FMEvent& event);

    QFile m_file;
    QTextStream m_textStream;
    quint8 m_flags;
};

#endif  // LOGGER_P_H
