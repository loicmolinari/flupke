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

#ifndef METRICS_P_H
#define METRICS_P_H

#include <QuickenPerf/metrics.h>

#include <sys/times.h>

#include <QtCore/QElapsedTimer>

#include <QuickenPerf/private/quickenmetricsglobal_p.h>

class QUICKEN_PERF_PRIVATE_EXPORT QPMetricsUtilsPrivate
{
public:
    QPMetricsUtilsPrivate();
    ~QPMetricsUtilsPrivate();

    void updateCpuUsage(QPMetrics* metrics);
    void updateProcStatMetrics(QPMetrics* metrics);

    char* m_buffer;
    QElapsedTimer m_cpuTimer;
    struct tms m_cpuTimes;
    clock_t m_cpuTicks;
    quint16 m_cpuOnlineCores;
    quint16 m_pageSize;
};

#endif  // METRICS_P_H
