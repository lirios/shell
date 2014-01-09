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

#include <QtCore/QDir>
#include <QtCore/QJsonObject>
#include <QtCore/QPluginLoader>

#include "elementfactory.h"
#include "element.h"
#include "hawaiishell.h"
#include "cmakedirs.h"

Q_GLOBAL_STATIC(ElementFactory, s_elementsFactory)

void ElementFactory::searchElements()
{
    QDir pluginsDir(QStringLiteral(INSTALL_LIBDIR) + "/hawaii/plugins/hawaii-shell/elements");
    for (const QString &fileName: pluginsDir.entryList(QDir::Files)) {
        QPluginLoader *loader = new QPluginLoader(pluginsDir.absoluteFilePath(fileName));
        QJsonObject metaData = loader->metaData();

        if (metaData.value("IID").toString() != QStringLiteral("HawaiiShell.Element")) {
            delete loader;
            continue;
        }

        QString name = metaData.value("className").toString();
        s_elementsFactory()->m_factories.insert(name, loader);
    }
}

void ElementFactory::cleanupElements()
{
    for (QPluginLoader *loader: s_elementsFactory()->m_factories)
        delete loader;
}

Element *ElementFactory::createElement(const QString &name, HawaiiShell *shell)
{
    if (!s_elementsFactory()->m_factories.contains(name)) {
        qWarning() << "Couldn't find a plugin for element" << name;
        return 0;
    }

    QPluginLoader *loader = s_elementsFactory()->m_factories.value(name);
    QObject *object = loader->instance();
    if (!object) {
        qWarning() << "Couldn't load plugin for element" << name
                   << loader->errorString();
        return 0;
    }

    Element *element = qobject_cast<Element *>(object);
    if (!element) {
        qWarning() << "Plugin" << name << "is not an Element subclass";
        return 0;
    }

    element->m_shell = shell;

    return element;
}
