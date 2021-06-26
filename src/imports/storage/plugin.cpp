// SPDX-FileCopyrightText: 2018 Pier Luigi Fiorini <pierluigi.fiorini@gmail.com>
//
// SPDX-License-Identifier: LGPL-3.0-or-later

#include <QtQml>

#include "storagedevice.h"
#include "storagemodel.h"

class StoragePlugin : public QQmlExtensionPlugin
{
    Q_OBJECT
    Q_PLUGIN_METADATA(IID "org.qt-project.Qt.QQmlExtensionInterface")
public:
    void registerTypes(const char *uri)
    {
        Q_ASSERT(QLatin1String(uri) == QLatin1String("Liri.Storage"));

        // @uri Liri.Storage
        qmlRegisterType<StorageModel>(uri, 1, 0, "StorageModel");
        qmlRegisterUncreatableType<StorageDevice>(uri, 1, 0, "StorageDevice",
                                                  QLatin1String("Cannot create StorageDevice object"));
    }
};

#include "plugin.moc"
