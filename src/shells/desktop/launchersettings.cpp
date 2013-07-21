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

#include "launchersettings.h"
#include "hawaiisettings.h"

LauncherSettings::LauncherSettings(QObject *parent)
    : QObject(parent)
{
    m_oldIconSize = iconSize();
    m_oldAlignment = alignment();
    m_oldFavApps = favoriteApps();

    connect(HawaiiSettings::self(), SIGNAL(configChanged()),
            this, SLOT(configChanged()));
}

LauncherSettings::IconSize LauncherSettings::iconSize() const
{
    return (LauncherSettings::IconSize)HawaiiSettings::self()->iconSize();
}

void LauncherSettings::setIconSize(IconSize value)
{
    HawaiiSettings::setIconSize((int)value);
}

LauncherSettings::Alignment LauncherSettings::alignment() const
{
    switch (HawaiiSettings::alignment()) {
    case 0:
        return LauncherSettings::LeftAlignment;
    case 1:
        return LauncherSettings::RightAlignment;
    default:
        break;
    }

    return LauncherSettings::BottomAlignment;
}

void LauncherSettings::setAlignment(LauncherSettings::Alignment value)
{
    HawaiiSettings::setAlignment((int)value);
}

QStringList LauncherSettings::favoriteApps() const
{
    return HawaiiSettings::favoriteApps();
}

void LauncherSettings::setFavoriteApps(const QStringList &value)
{
    HawaiiSettings::setFavoriteApps(value);
}

void LauncherSettings::configChanged()
{
    if (m_oldIconSize != iconSize()) {
        m_oldIconSize = iconSize();
        Q_EMIT iconSizeChanged();
    }

    if (m_oldAlignment != alignment()) {
        m_oldAlignment = alignment();
        Q_EMIT alignmentChanged();
    }

    if (m_oldFavApps != favoriteApps()) {
        m_oldFavApps = favoriteApps();
        Q_EMIT favoriteAppsChanged();
    }
}

#include "moc_launchersettings.cpp"
