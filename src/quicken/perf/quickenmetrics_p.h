// Copyright © 2017-2018 Loïc Molinari <loicm@loicm.fr>
// Copyright © 2016 Canonical Ltd.
//
// This file is part of Quicken, licensed under the MIT license. See the license
// file at project root for full information.

#ifndef METRICS_P_H
#define METRICS_P_H

#include <Quicken/quickenmetrics.h>

#include <sys/times.h>

#include <QtCore/QElapsedTimer>

#include <Quicken/private/quickenglobal_p.h>

class QUICKEN_PRIVATE_EXPORT QuickenMetricsUtilsPrivate
{
public:
    QuickenMetricsUtilsPrivate();
    ~QuickenMetricsUtilsPrivate();

    void updateCpuUsage(QuickenMetrics* metrics);
    void updateProcStatMetrics(QuickenMetrics* metrics);

    char* m_buffer;
    QElapsedTimer m_cpuTimer;
    struct tms m_cpuTimes;
    clock_t m_cpuTicks;
    quint16 m_cpuOnlineCores;
    quint16 m_pageSize;
};

#endif  // METRICS_P_H
