// Copyright © 2017-2018 Loïc Molinari <loicm@loicm.fr>
// Copyright © 2016 Canonical Ltd.
//
// This file is part of Quicken, licensed under the MIT license. See the license
// file at project root for full information.

#ifndef QUICKENGLOBAL_H
#define QUICKENGLOBAL_H

#include <QtCore/QtGlobal>

#if defined(QT_BUILD_QUICKEN_LIB)
#define QUICKEN_EXPORT Q_DECL_EXPORT
#else
#define QUICKEN_EXPORT Q_DECL_IMPORT
#endif

#endif  // QUICKENGLOBAL_H
