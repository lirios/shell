/****************************************************************************
 * This file is part of Hawaii Shell.
 *
 * Copyright (C) 2013 Pier Luigi Fiorini <pierluigi.fiorini@gmail.com>
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

#include <QtConfiguration/QConfiguration>

#include "launchersettings.h"

LauncherSettings::LauncherSettings(QObject *parent)
    : QObject(parent)
{
    m_iconSize = MediumIconSize;
    m_alignment = BottomAlignment;
    m_favApps.append("swordfish.desktop");
    m_favApps.append("hawaii-terminal.desktop");
    m_favApps.append("hawaii-system-preferences.desktop");

    // Configuration
    m_configuration = new QConfiguration(this, QStringLiteral("shell/launcher"));
}

LauncherSettings::~LauncherSettings()
{
    delete m_configuration;
}

LauncherSettings::IconSize LauncherSettings::iconSize() const
{
    return m_iconSize;
}

void LauncherSettings::setIconSize(IconSize value)
{
    if (m_iconSize != value) {
        m_iconSize = value;
        Q_EMIT iconSizeChanged(value);
    }
}

LauncherSettings::Alignment LauncherSettings::alignment() const
{
    return m_alignment;
}

void LauncherSettings::setAlignment(LauncherSettings::Alignment value)
{
    if (m_alignment != value) {
        m_alignment = value;
        Q_EMIT alignmentChanged(value);
    }
}

QStringList LauncherSettings::favoriteApps() const
{
    return m_favApps;
}

void LauncherSettings::setFavoriteApps(const QStringList &value)
{
    if (m_favApps != value) {
        m_favApps = value;
        Q_EMIT favoriteAppsChanged(value);
    }
}

#include "moc_launchersettings.cpp"
