// Copyright © 2018 Loïc Molinari <loicm@loicm.fr>
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

#include <QtQml/qqmlextensionplugin.h>

#include <Quicken/private/quickenboilerplate_p.h>

class QuickenItemsPlugin : public QQmlExtensionPlugin
{
    Q_OBJECT
    Q_PLUGIN_METADATA(IID QQmlExtensionInterface_iid)

public:
    QuickenItemsPlugin(QObject* parent = nullptr) : QQmlExtensionPlugin(parent) { }
    ~QuickenItemsPlugin() { }

    void registerTypes(const char* uri) Q_DECL_OVERRIDE {
        Q_ASSERT(QLatin1String(uri) == QLatin1String("Quicken.Items"));

#if !defined(QT_NO_DEBUG)
        qmlRegisterType<QuickenBoilerplate>(uri, 0, 1, "Boilerplate");
#endif
    }
};

#include "plugin.moc"
