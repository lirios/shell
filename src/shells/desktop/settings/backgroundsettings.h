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

#ifndef BACKGROUNDSETTINGS_H
#define BACKGROUNDSETTINGS_H

#include <QtCore/QObject>
#include <QtCore/QUrl>
#include <QtConfiguration/QConfiguration>
#include <QtGui/QColor>

class BackgroundSettings : public QObject
{
    Q_OBJECT
    Q_PROPERTY(BackgroundSettings::Type type READ type WRITE setType NOTIFY typeChanged)
    Q_PROPERTY(QColor primaryColor READ primaryColor WRITE setPrimaryColor NOTIFY primaryColorChanged)
    Q_PROPERTY(QColor secondaryColor READ secondaryColor WRITE setSecondaryColor NOTIFY secondaryColorChanged)
    Q_PROPERTY(BackgroundSettings::ColorShadingType colorShading READ colorShading WRITE setColorShading NOTIFY colorShadingChanged)
    Q_PROPERTY(QUrl wallpaperUrl READ wallpaperUrl WRITE setWallpaperUrl NOTIFY wallpaperUrlChanged)
    Q_ENUMS(Type ColorShadingType)
public:
    enum Type {
        ColorBackground = 0,
        WallpaperBackground
    };

    enum ColorShadingType {
        SolidColorShading = 0,
        HorizontalColorShading,
        VerticalColorShading
    };

    BackgroundSettings(QObject *parent = 0);
    ~BackgroundSettings();

    Type type() const;
    void setType(Type value);

    QColor primaryColor() const;
    void setPrimaryColor(const QColor &value);

    QColor secondaryColor() const;
    void setSecondaryColor(const QColor &value);

    ColorShadingType colorShading() const;
    void setColorShading(ColorShadingType value);

    QUrl wallpaperUrl() const;
    void setWallpaperUrl(const QUrl &value);

Q_SIGNALS:
    void typeChanged(const BackgroundSettings::Type &value);
    void primaryColorChanged(const QColor &value);
    void secondaryColorChanged(const QColor &value);
    void colorShadingChanged(const BackgroundSettings::ColorShadingType &value);
    void wallpaperUrlChanged(const QUrl &value);

private:
    QConfiguration *m_configuration;
    BackgroundSettings::Type m_type;
    QColor m_primaryColor;
    QColor m_secondaryColor;
    BackgroundSettings::ColorShadingType m_shading;
    QUrl m_url;
};

#endif // BACKGROUNDSETTINGS_H
