/****************************************************************************
 * This file is part of Hawaii Shell.
 *
 * Copyright (C) 2014 Pier Luigi Fiorini <pierluigi.fiorini@gmail.com>
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

#include <QtQml/QQmlExtensionPlugin>
#include <QtQml/QQmlComponent>

#include "dialogwindow.h"
#include "popupwindow.h"
#include "tooltipwindow.h"
#include "registrylistener.h"

class HawaiiShellComponentsPlugin : public QQmlExtensionPlugin
{
    Q_OBJECT
    Q_PLUGIN_METADATA(IID "org.qt-project.Qt.QQmlExtensionInterface/1.0")
public:
    void registerTypes(const char *uri);
    void initializeEngine(QQmlEngine *engine, const char *uri);
};

void HawaiiShellComponentsPlugin::registerTypes(const char *uri)
{
    Q_UNUSED(uri);
}

void HawaiiShellComponentsPlugin::initializeEngine(QQmlEngine *engine, const char *uri)
{
    Q_UNUSED(engine);
    Q_UNUSED(uri);

    RegistryListener::instance();

    // @uri Hawaii.Shell.Components.Private
    const char *privateUri = "Hawaii.Shell.Components.Private";
    qmlRegisterType<DialogWindow>(privateUri, 1, 0, "DialogWindow");
    qmlRegisterType<PopupWindow>(privateUri, 1, 0, "PopupWindow");
    qmlRegisterType<TooltipWindow>(privateUri, 1, 0, "TooltipWindow");
}

#include "plugin.moc"
