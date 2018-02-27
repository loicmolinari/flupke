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

#include "events_p.h"

#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <cstdio>

#include <QtCore/QElapsedTimer>

#include "quickenmetricsglobal_p.h"

const int bufferSize = 128;
const int bufferAlignment = 64;

QMEventUtils::QMEventUtils()
    : d_ptr(new QMEventUtilsPrivate)
{
}

QMEventUtilsPrivate::QMEventUtilsPrivate()
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

QMEventUtils::~QMEventUtils()
{
    delete d_ptr;
}

QMEventUtilsPrivate::~QMEventUtilsPrivate()
{
    free(m_buffer);
}

void QMEventUtils::updateProcessEvent(QMEvent* event)
{
    DASSERT(event);
    Q_D(QMEventUtils);

    event->type = QMEvent::Process;
    event->timeStamp = QMEventUtils::timeStamp();
    d->updateCpuUsage(event);
    d->updateProcStatMetrics(event);
}

void QMEventUtilsPrivate::updateCpuUsage(QMEvent* event)
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
        event->process.cpuUsage = ((userTime + systemTime) * 100) / (ticks * m_cpuOnlineCores);
        m_cpuTimer.start();
        memcpy(&m_cpuTimes, &newCpuTimes, sizeof(struct tms));
        m_cpuTicks = newTicks;
    }
}

void QMEventUtilsPrivate::updateProcStatMetrics(QMEvent* event)
{
    int fd = open("/proc/self/stat", O_RDONLY);
    if (fd == -1) {
        DWARN("EventUtils: can't open '/proc/self/stat'");
        return;
    }
    int readSize;
    if ((readSize = read(fd, m_buffer, bufferSize)) == 0) {
        DWARN("EventUtils: can't read '/proc/self/stat'");
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

    event->process.vszMemory = vsize >> 10;
    event->process.rssMemory = (rss * m_pageSize) >> 10;
    event->process.threadCount = threadCount;

    close(fd);
}

// static.
quint64 QMEventUtils::timeStamp()
{
    static QElapsedTimer timer;

    if (Q_LIKELY(timer.isValid())) {
        return timer.nsecsElapsed();
    } else {
        timer.start();
        return 0;
    }
}
