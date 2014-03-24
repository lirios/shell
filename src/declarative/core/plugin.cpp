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

#include <HawaiiShell/ContainmentItem>
#include <HawaiiShell/ElementItem>

#include "packagesmodelitem.h"
#include "packagesmodel.h"

using namespace Hawaii::Shell;

class HawaiiShellCorePlugin : public QQmlExtensionPlugin
{
    Q_OBJECT
    Q_PLUGIN_METADATA(IID "org.qt-project.Qt.QQmlExtensionInterface/1.0")
public:
    void registerTypes(const char *uri);
};

void HawaiiShellCorePlugin::registerTypes(const char *uri)
{
    // @uri Hawaii.Shell.Core
    qmlRegisterUncreatableType<ContainmentItem>(uri, 1, 0, "Containment",
                                                QStringLiteral("Do not create Containment"));
    qmlRegisterUncreatableType<ElementItem>(uri, 1, 0, "Element",
                                            QStringLiteral("Do not create Element"));
    qmlRegisterUncreatableType<PackagesModelItem>(uri, 1, 0, "PackagesModelItem",
                                                  QStringLiteral("Do not create PackagesModelItem"));
    qmlRegisterType<PackagesModel>(uri, 1, 0, "PackagesModel");
}

#include "plugin.moc"
