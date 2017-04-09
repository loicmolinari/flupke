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

#ifndef FLUPKEMETRICSGLOBAL_H
#define FLUPKEMETRICSGLOBAL_H

#include <QtCore/QtGlobal>

#if defined(QT_BUILD_FLUPKEMETRICS_LIB)
#define FLUPKE_METRICS_EXPORT Q_DECL_EXPORT
#else
#define FLUPKE_METRICS_EXPORT Q_DECL_IMPORT
#endif

#endif  // FLUPKEMETRICSGLOBAL_H
