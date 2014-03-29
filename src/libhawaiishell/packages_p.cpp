/****************************************************************************
 * This file is part of Hawaii Shell.
 *
 * Copyright (C) 2012-2014 Pier Luigi Fiorini <pierluigi.fiorini@gmail.com>
 *
 * Author(s):
 *    Pier Luigi Fiorini
 *
 * $BEGIN_LICENSE:LGPL2.1+$
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License as published by
 * the Free Software Foundation, either version 2.1 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 *
 * $END_LICENSE$
 ***************************************************************************/

#include "pluginmetadata.h"
#include "packages_p.h"

namespace Hawaii {

namespace Shell {

void BasePackage::initializePackage(Package *package)
{
    // Metadata
    package->addFileDefinition("metadata",
                               QStringLiteral("metadata.json"),
                               tr("Metadata file"));
    package->setRequired("metadata", true);
}

void MainScriptPackage::initializePackage(Package *package)
{
    // Default main script, this can be changed by packages themselves
    package->addFileDefinition("mainscript", QStringLiteral("ui/main.qml"),
                               tr("Main script file"));
    package->setRequired("mainscript", true);
}

void MainScriptPackage::pathChanged(Package *package)
{
    if (package->path().isEmpty())
        return;

    PluginMetadata metadata(package->path() + "/metadata.json");
    const QString mainScript = metadata.mainScript();
    if (!mainScript.isEmpty())
        package->addFileDefinition("mainscript", mainScript, tr("Main script file"));
}

void BackgroundPackage::initializePackage(Package *package)
{
    package->setDefaultPackageRoot(QStringLiteral("hawaii/backgrounds/"));

    // User interface
    package->addDirectoryDefinition("ui", QStringLiteral("ui"), tr("User Interface"));

    // System Preferences views
    package->addDirectoryDefinition("preferences", QStringLiteral("preferences"),
                                    tr("Code and resources for the preferences user interface"));
    package->addFileDefinition("preferencesview",
                               QStringLiteral("preferences/Preferences.qml"),
                               tr("User interface for System Preferences"));
    package->setMimeTypes("preferencesview", QStringList() << QStringLiteral("text/x-qml"));
}

void ElementPackage::initializePackage(Package *package)
{
    package->setDefaultPackageRoot(QStringLiteral("hawaii/elements/"));

    // User interface
    package->addDirectoryDefinition("ui", QStringLiteral("ui"), tr("User Interface"));
}

void ContainmentPackage::initializePackage(Package *package)
{
    package->setDefaultPackageRoot(QStringLiteral("hawaii/containments/"));

    // User interface
    package->addDirectoryDefinition("ui", QStringLiteral("ui"), tr("User Interface"));
}

void ShellPackage::initializePackage(Package *package)
{
    package->setDefaultPackageRoot(QStringLiteral("hawaii/shells/"));

    // Views
    package->addDirectoryDefinition("views", QStringLiteral("views"), tr("User interface for the views"));
    package->setMimeTypes("views", QStringList() << QStringLiteral("text/x-qml"));

    // Desktop configuration view
    package->addFileDefinition("desktopconfigurationview",
                               QStringLiteral("configuration/DesktopConfigView.qml"),
                               tr("Configuration User Interface"));
    package->setMimeTypes("desktopconfigurationview", QStringList() << QStringLiteral("text/x-qml"));

    // Panel configuration view
    package->addFileDefinition("panelconfigurationview",
                               QStringLiteral("configuration/PanelConfigView.qml"),
                               tr("Configuration User Interface"));
    package->setMimeTypes("panelconfigurationview", QStringList() << QStringLiteral("text/x-qml"));

    // Loader
    package->addFileDefinition("loader", QStringLiteral("loader.qml"),
                               tr("Shell loader"));
    package->setMimeTypes("loader", QStringList() << QStringLiteral("text/x-qml"));

    // Default layout
    package->addFileDefinition("defaultlayout", QStringLiteral("layout.js"),
                               tr("Default layout script"));
    package->setMimeTypes("defaultlayout", QStringList() << QStringLiteral("application/javascript"));

    // Elements
    package->addFileDefinition("elementerror", QStringLiteral("element/ElementError.qml"),
                               tr("Error message for elements that fail to load"));
}

void LookAndFeelPackage::initializePackage(Package *package)
{
    package->setDefaultPackageRoot(QStringLiteral("hawaii/lookandfeel/"));

    // Previews
    package->addDirectoryDefinition("previews", QStringLiteral("previews"),
                                    tr("Preview images"));
    package->addFileDefinition("authenticationpreview", QStringLiteral("previews/authentication.png"),
                               tr("Preview for the authentication user interface"));
    package->addFileDefinition("lockscreenpreview", QStringLiteral("previews/lockscreen.png"),
                               tr("Preview for the Lock Screen"));

    // Authentication
    package->addDirectoryDefinition("authenticationui", QStringLiteral("authentication"),
                                    tr("Authentication user interface"));
    package->addFileDefinition("authentication", QStringLiteral("authentication/Authentication.qml"),
                               tr("Main authentication user interface file"));

    // Lock Screen
    package->addDirectoryDefinition("lockscreenui", QStringLiteral("lockscreen"),
                                    tr("Lock screen user interface"));
    package->addFileDefinition("lockscreen", QStringLiteral("lockscreen/LockScreen.qml"),
                               tr("Main lock screen user interface file"));
}

void PreferencesPackage::initializePackage(Package *package)
{
    package->setDefaultPackageRoot(QStringLiteral("hawaii/preferences/"));

    // User interface
    package->addDirectoryDefinition("ui", QStringLiteral("ui"), tr("User Interface"));
}

} // namespace Shell

} // namespace Hawaii
