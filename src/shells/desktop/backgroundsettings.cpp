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

#include "backgroundsettings.h"
#include "hawaiisettings.h"

BackgroundSettings::BackgroundSettings(QObject *parent)
    : QObject(parent)
{
    m_oldType = type();
    m_oldPrimaryColor = primaryColor();
    m_oldSecondaryColor = secondaryColor();
    m_oldShading = colorShading();
    m_oldUrl = wallpaperUrl();

    connect(HawaiiSettings::self(), SIGNAL(configChanged()),
            this, SLOT(configChanged()));
}

BackgroundSettings::Type BackgroundSettings::type() const
{
    switch (HawaiiSettings::type()) {
    case 0:
        return BackgroundSettings::ColorBackground;
    default:
        break;
    }

    return BackgroundSettings::WallpaperBackground;
}

void BackgroundSettings::setType(Type value)
{
    HawaiiSettings::setType(value);
}

QColor BackgroundSettings::primaryColor() const
{
    return HawaiiSettings::primaryColor();
}

void BackgroundSettings::setPrimaryColor(const QColor &value)
{
    HawaiiSettings::setPrimaryColor(value);
}

QColor BackgroundSettings::secondaryColor() const
{
    return HawaiiSettings::secondaryColor();
}

void BackgroundSettings::setSecondaryColor(const QColor &value)
{
    HawaiiSettings::setSecondaryColor(value);
}

BackgroundSettings::ColorShadingType BackgroundSettings::colorShading() const
{
    switch (HawaiiSettings::colorShadingType()) {
    case 1:
        return BackgroundSettings::HorizontalColorShading;
    case 2:
        return BackgroundSettings::VerticalColorShading;
    default:
        break;
    }

    return BackgroundSettings::SolidColorShading;
}

void BackgroundSettings::setColorShading(BackgroundSettings::ColorShadingType value)
{
    HawaiiSettings::setColorShadingType(value);
}

QUrl BackgroundSettings::wallpaperUrl() const
{
    return HawaiiSettings::wallpaperUrl();
}

void BackgroundSettings::setWallpaperUrl(const QUrl &value)
{
    HawaiiSettings::setWallpaperUrl(value);
}

void BackgroundSettings::configChanged()
{
    if (m_oldType != type()) {
        m_oldType = type();
        Q_EMIT typeChanged();
    }

    if (m_oldPrimaryColor != primaryColor()) {
        m_oldPrimaryColor = primaryColor();
        Q_EMIT primaryColorChanged();
    }

    if (m_oldSecondaryColor != secondaryColor()) {
        m_oldSecondaryColor = secondaryColor();
        Q_EMIT secondaryColorChanged();
    }

    if (m_oldShading != colorShading()) {
        m_oldShading = colorShading();
        Q_EMIT colorShadingChanged();
    }

    if (m_oldUrl != wallpaperUrl()) {
        m_oldUrl = wallpaperUrl();
        Q_EMIT wallpaperUrlChanged();
    }
}

#include "moc_backgroundsettings.cpp"
