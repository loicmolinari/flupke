// Copyright © 2017-2018 Loïc Molinari <loicm@loicm.fr>
// Copyright © 2016 Canonical Ltd.
//
// This file is part of Quicken, licensed under the MIT license. See the license
// file at project root for full information.

#include "quickenmetrics_p.h"

#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <cstdio>

#include <QtCore/QElapsedTimer>

const int bufferSize = 128;
const int bufferAlignment = 64;

QuickenMetricsUtils::QuickenMetricsUtils()
    : d_ptr(new QuickenMetricsUtilsPrivate)
{
}

QuickenMetricsUtilsPrivate::QuickenMetricsUtilsPrivate()
{
#if !defined(QT_NO_DEBUG)
    ASSERT(m_buffer = static_cast<char*>(alignedAlloc(bufferAlignment, bufferSize)));
#else
    m_buffer = static_cast<char*>(alignedAlloc(bufferAlignment, bufferSize));
#endif
    m_cpuTimer.start();
    m_cpuTicks = times(&m_cpuTimes);
    m_cpuOnlineCores = sysconf(_SC_NPROCESSORS_ONLN);
    m_pageSize = sysconf(_SC_PAGESIZE);
}

QuickenMetricsUtils::~QuickenMetricsUtils()
{
    delete d_ptr;
}

QuickenMetricsUtilsPrivate::~QuickenMetricsUtilsPrivate()
{
    free(m_buffer);
}

void QuickenMetricsUtils::updateProcessMetrics(QuickenMetrics* metrics)
{
    DASSERT(metrics);
    Q_D(QuickenMetricsUtils);

    metrics->type = QuickenMetrics::Process;
    metrics->timeStamp = QuickenMetricsUtils::timeStamp();
    d->updateCpuUsage(metrics);
    d->updateProcStatMetrics(metrics);
}

void QuickenMetricsUtilsPrivate::updateCpuUsage(QuickenMetrics* metrics)
{
    // times() is a Linux syscall giving CPU times used by the process. The
    // granularity of the unit returned by the (some?) kernel (clock ticks)
    // prevents us from getting precise timings at a high frequency, so we have
    // to throttle to 200 ms (5 Hz).
    const qint64 throttlingFrequency = 200;
    if (m_cpuTimer.elapsed() > throttlingFrequency) {
        struct tms newCpuTimes;
        const clock_t newTicks = times(&newCpuTimes);
        const clock_t ticks = newTicks - m_cpuTicks;
        const clock_t userTime = newCpuTimes.tms_utime - m_cpuTimes.tms_utime;
        const clock_t systemTime = newCpuTimes.tms_stime - m_cpuTimes.tms_stime;
        metrics->process.cpuUsage = ((userTime + systemTime) * 100) / (ticks * m_cpuOnlineCores);
        m_cpuTimer.start();
        memcpy(&m_cpuTimes, &newCpuTimes, sizeof(struct tms));
        m_cpuTicks = newTicks;
    }
}

void QuickenMetricsUtilsPrivate::updateProcStatMetrics(QuickenMetrics* metrics)
{
    int fd = open("/proc/self/stat", O_RDONLY);
    if (fd == -1) {
        DWARN("MetricsUtils: can't open '/proc/self/stat'");
        return;
    }
    int readSize;
    if ((readSize = read(fd, m_buffer, bufferSize)) == 0) {
        DWARN("MetricsUtils: can't read '/proc/self/stat'");
        close(fd);
        return;
    }

    // Entries starting from 1 (as listed by 'man proc').
    const int numThreadsEntry = 20;
    const int vsizeEntry = 23;
    const int rssEntry = 24;
    const int lastEntry = rssEntry;

    // Get the indices of num_threads, vsize and rss entries and check if
    // the buffer is big enough.
    int sourceIndex = 0, spaceCount = 0;
    quint16 entryIndices[lastEntry + 1];
    entryIndices[sourceIndex] = 0;
    while (spaceCount < lastEntry) {
        if (sourceIndex < readSize) {
            if (m_buffer[sourceIndex++] == ' ') {
                entryIndices[++spaceCount] = sourceIndex;
            }
        } else {
            DASSERT(readSize == bufferSize); // Missing entries in /proc/self/stat.
            DNOT_REACHED();  // Consider increasing bufferSize.
            close(fd);
            return;
        }
    }

    unsigned long vsize;
    long threadCount, rss;
#if !defined(QT_NO_DEBUG)
    int value =  sscanf(&m_buffer[entryIndices[numThreadsEntry-1]], "%ld", &threadCount);
    ASSERT(value == 1);
    value =  sscanf(&m_buffer[entryIndices[vsizeEntry-1]], "%lu %ld", &vsize, &rss);
    ASSERT(value == 2);
#else
    sscanf(&m_buffer[entryIndices[numThreadsEntry-1]], "%ld", &threadCount);
    sscanf(&m_buffer[entryIndices[vsizeEntry-1]], "%lu %ld", &vsize, &rss);
#endif

    metrics->process.vszMemory = vsize >> 10;
    metrics->process.rssMemory = (rss * m_pageSize) >> 10;
    metrics->process.threadCount = threadCount;

    close(fd);
}

// static.
quint64 QuickenMetricsUtils::timeStamp()
{
    static QElapsedTimer timer;

    if (Q_LIKELY(timer.isValid())) {
        return timer.nsecsElapsed();
    } else {
        timer.start();
        return 0;
    }
}
