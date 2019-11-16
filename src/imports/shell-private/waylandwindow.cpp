/****************************************************************************
 * This file is part of Liri.
 *
 * Copyright (C) 2019 Pier Luigi Fiorini <pierluigi.fiorini@gmail.com>
 *
 * $BEGIN_LICENSE:GPL3+$
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
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

#include <QStandardPaths>

#include <LiriXdg/DesktopFile>

#include "waylandwindow.h"

WaylandWindow::WaylandWindow(QObject *parent)
    : QObject(parent)
{
}

QString WaylandWindow::appId() const
{
    return m_appId;
}

void WaylandWindow::setAppId(const QString &appId)
{
    if (m_appId == appId)
        return;

    m_appId = appId;
    emit appIdChanged();

    updateIconName();
}

QString WaylandWindow::iconName() const
{
    return m_iconName;
}

void WaylandWindow::updateIconName()
{
    QString fileName = m_appId + QStringLiteral(".desktop");
    QString path =
            QStandardPaths::locate(QStandardPaths::ApplicationsLocation,
                                   fileName);
    if (!path.isEmpty()) {
        Liri::DesktopFile desktopFile(path);
        if (desktopFile.iconName() != m_iconName) {
            m_iconName = desktopFile.iconName();
            emit iconNameChanged();
        }
    }
}
