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

#ifndef EVENTS_P_H
#define EVENTS_P_H

#include <QuickenMetrics/events.h>

#include <sys/times.h>

#include <QtCore/QElapsedTimer>

#include <QuickenMetrics/private/quickenmetricsglobal_p.h>

class QUICKEN_METRICS_PRIVATE_EXPORT QMEventUtilsPrivate
{
public:
    QMEventUtilsPrivate();
    ~QMEventUtilsPrivate();

    void updateCpuUsage(QMEvent* event);
    void updateProcStatMetrics(QMEvent* event);

    char* m_buffer;
    QElapsedTimer m_cpuTimer;
    struct tms m_cpuTimes;
    clock_t m_cpuTicks;
    quint16 m_cpuOnlineCores;
    quint16 m_pageSize;
};

#endif  // EVENTS_P_H
