// Copyright © 2018 Loïc Molinari <loicm@loicm.fr>
//
// This file is part of Quicken, licensed under the MIT license. See the license
// file at project root for full information.

#include <QtQml/qqmlextensionplugin.h>

#include <Quicken/Quicken>  // FIXME

class QuickenPerfPlugin : public QQmlExtensionPlugin
{
    Q_OBJECT
    Q_PLUGIN_METADATA(IID QQmlExtensionInterface_iid)

public:
    QuickenPerfPlugin(QObject* parent = nullptr) : QQmlExtensionPlugin(parent) { }
    ~QuickenPerfPlugin() { }

    void registerTypes(const char* uri) Q_DECL_OVERRIDE {
        Q_ASSERT(QLatin1String(uri) == QLatin1String("Quicken.Perf"));
        Q_UNUSED(uri);

        // FIXME Register types here.
    }
};

#include "plugin.moc"
