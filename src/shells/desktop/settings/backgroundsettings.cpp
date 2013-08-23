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

#include <QtCore/QStandardPaths>

#include "backgroundsettings.h"
#include "cmakedirs.h"

BackgroundSettings::BackgroundSettings(QObject *parent)
    : QObject(parent)
{
    // Set default values
    m_type = BackgroundSettings::WallpaperBackground;
    m_primaryColor = QColor(51, 102, 153);
    m_secondaryColor = QColor(46, 93, 140);
    m_shading = BackgroundSettings::SolidColorShading;
    m_url = QUrl::fromLocalFile(QStringLiteral("%1/backgrounds/hawaii/Also Calm.png")
                                .arg(INSTALL_DATADIR));

    // Configuration
    m_configuration = new QConfiguration(this, QStringLiteral("shell/background"));
}

BackgroundSettings::~BackgroundSettings()
{
    delete m_configuration;
}

BackgroundSettings::Type BackgroundSettings::type() const
{
    return m_type;
}

void BackgroundSettings::setType(Type value)
{
    if (m_type != value) {
        m_type = value;
        Q_EMIT typeChanged(value);
    }
}

QColor BackgroundSettings::primaryColor() const
{
    return m_primaryColor;
}

void BackgroundSettings::setPrimaryColor(const QColor &value)
{
    if (m_primaryColor != value) {
        m_primaryColor = value;
        Q_EMIT primaryColorChanged(value);
    }
}

QColor BackgroundSettings::secondaryColor() const
{
    return m_secondaryColor;
}

void BackgroundSettings::setSecondaryColor(const QColor &value)
{
    if (m_secondaryColor != value) {
        m_secondaryColor = value;
        Q_EMIT secondaryColorChanged(value);
    }
}

BackgroundSettings::ColorShadingType BackgroundSettings::colorShading() const
{
    return m_shading;
}

void BackgroundSettings::setColorShading(BackgroundSettings::ColorShadingType value)
{
    if (m_shading != value) {
        m_shading = value;
        Q_EMIT colorShadingChanged(value);
    }
}

QUrl BackgroundSettings::wallpaperUrl() const
{
    return m_url;
}

void BackgroundSettings::setWallpaperUrl(const QUrl &value)
{
    if (m_url != value) {
        m_url = value;
        Q_EMIT wallpaperUrlChanged(value);
    }
}

#include "moc_backgroundsettings.cpp"
