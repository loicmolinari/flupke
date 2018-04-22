// Copyright © 2018 Loïc Molinari <loicm@loicm.fr>
//
// This file is part of Quicken, licensed under the MIT license. See the license
// file at project root for full information.

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
