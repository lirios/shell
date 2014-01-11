/****************************************************************************
 * This file is part of Hawaii Shell.
 *
 * Copyright (C) 2012-2013 Pier Luigi Fiorini <pierluigi.fiorini@gmail.com>
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

#include <QtCore/QDebug>

#include "cmakedirs.h"
#include "elementfactory.h"

Q_GLOBAL_STATIC(ElementFactory, s_elementsFactory)

void ElementFactory::searchElements()
{
    qDebug() << "-> Looking for elements...";

    QDir elementsDir(QStringLiteral(INSTALL_DATADIR) + QStringLiteral("/hawaii/elements"));
    for (const QString &dirName: elementsDir.entryList(QDir::Dirs | QDir::NoDotAndDotDot)) {
        const QString &metadataFileName =
                elementsDir.absoluteFilePath(dirName + QStringLiteral("/metadata.desktop"));

        // Try to load plugin metadata, return if it's not valid
        Hawaii::Shell::PluginMetadata metadata(metadataFileName);
        if (!metadata.isValid())
            continue;

        // Check whether we already added this element
        const QString &name = metadata.internalName();
        if (s_elementsFactory()->m_factories.contains(name))
            continue;

        // Register element
        s_elementsFactory()->m_factories.insert(name, new ElementInfo(metadataFileName));
        qDebug() << "Registered" << name;
    }
}

void ElementFactory::cleanupElements()
{
    for (const QString &key: s_elementsFactory()->m_factories.keys())
        delete s_elementsFactory()->m_factories.take(key);
}

Hawaii::Shell::Element *ElementFactory::createElement(const QString &name)
{
    if (!s_elementsFactory()->m_factories.contains(name)) {
        qWarning() << "Element" << name << "is not registered";
        return nullptr;
    }

    ElementInfo *info = s_elementsFactory()->m_factories.value(name);
    return info->createElement();
}
