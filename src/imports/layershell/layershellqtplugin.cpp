/*
 *   SPDX-FileCopyrightText: 2023 Aleix Pol Gonzalez <aleix.pol_gonzalez@mercedes-benz.com>
 *
 *   SPDX-License-Identifier: LGPL-3.0-or-later
 */

#include <QQmlExtensionPlugin>
#include <qqml.h>

#include <LayerShellQt/Window>

QML_DECLARE_TYPEINFO(LayerShellQt::Window, QML_HAS_ATTACHED_PROPERTIES)

class Plugin : public QQmlExtensionPlugin
{
    Q_OBJECT
    Q_PLUGIN_METADATA(IID QQmlExtensionInterface_iid)
public:
    void registerTypes(const char *uri) override {
        Q_ASSERT(QLatin1String(uri) == QLatin1String("Liri.Shell.LayerShell"));
        qmlRegisterType<LayerShellQt::Window>(uri, 1, 0, "Window");
    }
};

#include "layershellqtplugin.moc"
