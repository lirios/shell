/****************************************************************************
 * This file is part of Hawaii Shell.
 *
 * Copyright (C) 2014 Pier Luigi Fiorini <pierluigi.fiorini@gmail.com>
 *
 * Author(s):
 *    Pier Luigi Fiorini
 *
 * $BEGIN_LICENSE:GPL2+$
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 *
 * $END_LICENSE$
 ***************************************************************************/

#include <QtCore/QDebug>
#include <QtCore/QElapsedTimer>
#include <QtCore/QFileInfo>
#include <QtQml/QQmlComponent>
#include <QtQml/QQmlEngine>
#include <QtQuick/QQuickItem>

#include "elementinfo.h"
#include "shellmanager.h"

ElementInfo::ElementInfo(const QString &fileName)
{
    m_metadata.load(fileName);

    QFileInfo info(fileName);
    m_dir = info.dir().absoluteFilePath(QStringLiteral("contents"));
}

Hawaii::Shell::PluginMetadata ElementInfo::metadata() const
{
    return m_metadata;
}

QDir ElementInfo::directory() const
{
    return m_dir;
}

Hawaii::Shell::Element *ElementInfo::createElement()
{
    QElapsedTimer timer;
    timer.start();

    QString fileName = m_metadata.mainScript();
    QString path = m_dir.absoluteFilePath(fileName);

    qDebug() << "-> Loading element" << m_metadata.internalName();

    QQmlEngine *engine = ShellManager::instance()->corona()->engine();
    QQmlComponent component(engine);
    component.loadUrl(QUrl::fromLocalFile(path));
    if (!component.isReady()) {
        for (QQmlError error: component.errors())
            qWarning() << "  ERROR:" << error.toString();
        return nullptr;
    }

    QObject *object = component.beginCreate(engine->rootContext());
    Hawaii::Shell::Element *element = qobject_cast<Hawaii::Shell::Element *>(object);
    if (!element) {
        qWarning() << "  Element" << m_metadata.internalName() << "is invalid";
        delete object;
        return nullptr;
    }

    component.completeCreate();

    qDebug() << "  Element created in" << timer.elapsed() << "ms";

    return element;
}
