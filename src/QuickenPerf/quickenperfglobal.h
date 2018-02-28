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

#ifndef QUICKENPERFGLOBAL_H
#define QUICKENPERFGLOBAL_H

#include <QtCore/QtGlobal>

#if defined(QT_BUILD_QUICKENPERF_LIB)
#define QUICKEN_PERF_EXPORT Q_DECL_EXPORT
#else
#define QUICKEN_PERF_EXPORT Q_DECL_IMPORT
#endif

#endif  // QUICKENPERFGLOBAL_H
