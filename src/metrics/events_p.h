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

#ifndef EVENTS_P_H
#define EVENTS_P_H

#include <FlupkeMetrics/events.h>

#include <sys/times.h>

#include <QtCore/QElapsedTimer>

#include <FlupkeMetrics/private/flupkemetricsglobal_p.h>

class FLUPKE_METRICS_PRIVATE_EXPORT FMEventUtilsPrivate
{
public:
    FMEventUtilsPrivate();
    ~FMEventUtilsPrivate();

    void updateCpuUsage(FMEvent* event);
    void updateProcStatMetrics(FMEvent* event);

    char* m_buffer;
    QElapsedTimer m_cpuTimer;
    struct tms m_cpuTimes;
    clock_t m_cpuTicks;
    quint16 m_cpuOnlineCores;
    quint16 m_pageSize;
};

#endif  // EVENTS_P_H
