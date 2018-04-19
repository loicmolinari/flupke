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

#ifndef QUICKENGLOBAL_P_H
#define QUICKENGLOBAL_P_H

#include <Quicken/quickenglobal.h>

// Logging macros, debug macros are compiled out for release builds.
#define LOG(...) qDebug(__VA_ARGS__)
#define WARN(...) qWarning(__VA_ARGS__)
#define ASSERT(cond) do { if (Q_UNLIKELY(!(cond))) \
    qFatal("Assertion `"#cond"' failed in file %s, line %d", __FILE__, __LINE__); } while (0)
#define ASSERT_X(cond,message) do { if (Q_UNLIKELY(!(cond))) \
    qFatal("Assertion "#message" failed in file %s, line %d", __FILE__, __LINE__); } while (0)
#define NOT_REACHED() \
    qFatal("Assertion `not reached' failed in file %s, line %d", __FILE__, __LINE__);

#if !defined(QT_NO_DEBUG)
#define DLOG(...) LOG(__VA_ARGS__)
#define DWARN(...) WARN(__VA_ARGS__)
#define DNOT_REACHED(...) NOT_REACHED()
#define DASSERT(cond) ASSERT(cond)
#define DASSERT_X(cond,message) ASSERT(cond, message)
#else
#define DLOG(...) qt_noop()
#define DWARN(...) qt_noop()
#define DNOT_REACHED(...) qt_noop()
#define DASSERT(cond) qt_noop()
#define DASSERT_X(cond,message) qt_noop()
#endif

// Compile-time constant representing the number of elements in an array.
template<typename T, size_t N> constexpr size_t ARRAY_SIZE(T (&)[N]) { return N; }

#define IS_POWER_OF_TWO(n) !((n) & ((n) - 1))

#define QUICKEN_PRIVATE_EXPORT QUICKEN_EXPORT

// Allocates size bytes and returns an pointer to the aligned memory. alignment
// must be a power-of-two and size a multiple of alignment.
void* alignedAlloc(size_t alignment, size_t size);

#endif  // QUICKENGLOBAL_P_H
