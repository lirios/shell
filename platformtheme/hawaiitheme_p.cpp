/****************************************************************************
 * This file is part of Hawaii.
 *
 * Copyright (C) 2015 Pier Luigi Fiorini
 *
 * Author(s):
 *    Pier Luigi Fiorini <pierluigi.fiorini@gmail.com>
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

#include <QtCore/QSettings>
#include <QtCore/QStandardPaths>
#include <QtGui/QPalette>
#include <qpa/qplatformtheme.h>

#include "hawaiitheme_p.h"

HawaiiThemePrivate::HawaiiThemePrivate()
{
}

void HawaiiThemePrivate::readPalette(const QString &scheme, QPalette *pal)
{
    QString fileName = QStandardPaths::locate(QStandardPaths::GenericDataLocation,
                                              QString("color-schemes/%1.colors").arg(scheme));
    if (fileName.isEmpty())
        return;

    QSettings colorScheme(fileName, QSettings::IniFormat);

    if (!readColor(pal, QPalette::Button, colorScheme.value(QStringLiteral("Colors:Button/BackgroundNormal")))) {
        // kcolorscheme.cpp: SetDefaultColors
        const QColor defaultWindowBackground(214, 210, 208);
        const QColor defaultButtonBackground(223, 220, 217);
        *pal = QPalette(defaultButtonBackground, defaultWindowBackground);
        return;
    }

    readColor(pal, QPalette::Window, colorScheme.value(QStringLiteral("Colors:Window/BackgroundNormal")));
    readColor(pal, QPalette::Text, colorScheme.value(QStringLiteral("Colors:View/ForegroundNormal")));
    readColor(pal, QPalette::WindowText, colorScheme.value(QStringLiteral("Colors:Window/ForegroundNormal")));
    readColor(pal, QPalette::Base, colorScheme.value(QStringLiteral("Colors:View/BackgroundNormal")));
    readColor(pal, QPalette::Highlight, colorScheme.value(QStringLiteral("Colors:Selection/BackgroundNormal")));
    readColor(pal, QPalette::HighlightedText, colorScheme.value(QStringLiteral("Colors:Selection/ForegroundNormal")));
    readColor(pal, QPalette::AlternateBase, colorScheme.value(QStringLiteral("Colors:View/BackgroundAlternate")));
    readColor(pal, QPalette::ButtonText, colorScheme.value(QStringLiteral("Colors:Button/ForegroundNormal")));
    readColor(pal, QPalette::Link, colorScheme.value(QStringLiteral("Colors:View/ForegroundLink")));
    readColor(pal, QPalette::LinkVisited, colorScheme.value(QStringLiteral("Colors:View/ForegroundVisited")));
    readColor(pal, QPalette::ToolTipBase, colorScheme.value(QStringLiteral("Colors:Tooltip/BackgroundNormal")));
    readColor(pal, QPalette::ToolTipText, colorScheme.value(QStringLiteral("Colors:Tooltip/ForegroundNormal")));

    // Set all color roles to "normal" colors, but calculate disabled colors
    const QColor button = pal->color(QPalette::Button);
    int h, s, v;
    button.getHsv(&h, &s, &v);

    const QBrush whiteBrush = QBrush(Qt::white);
    const QBrush buttonBrush = QBrush(button);
    const QBrush buttonBrushDark = QBrush(button.darker(v > 128 ? 200 : 50));
    const QBrush buttonBrushDark150 = QBrush(button.darker(v > 128 ? 150 : 75));
    const QBrush buttonBrushLight150 = QBrush(button.lighter(v > 128 ? 150 : 75));
    const QBrush buttonBrushLight = QBrush(button.lighter(v > 128 ? 200 : 50));

    pal->setBrush(QPalette::Disabled, QPalette::WindowText, buttonBrushDark);
    pal->setBrush(QPalette::Disabled, QPalette::ButtonText, buttonBrushDark);
    pal->setBrush(QPalette::Disabled, QPalette::Button, buttonBrush);
    pal->setBrush(QPalette::Disabled, QPalette::Text, buttonBrushDark);
    pal->setBrush(QPalette::Disabled, QPalette::BrightText, whiteBrush);
    pal->setBrush(QPalette::Disabled, QPalette::Base, buttonBrush);
    pal->setBrush(QPalette::Disabled, QPalette::Window, buttonBrush);
    pal->setBrush(QPalette::Disabled, QPalette::Highlight, buttonBrushDark150);
    pal->setBrush(QPalette::Disabled, QPalette::HighlightedText, buttonBrushLight150);

    // Set calculated colors for all groups
    pal->setBrush(QPalette::Light, buttonBrushLight);
    pal->setBrush(QPalette::Midlight, buttonBrushLight150);
    pal->setBrush(QPalette::Mid, buttonBrushDark150);
    pal->setBrush(QPalette::Dark, buttonBrushDark);
}

void HawaiiThemePrivate::refresh()
{
    resources.clear();

    // Palette
    QPalette systemPalette = QPalette();
    readPalette(QStringLiteral("Breeze"), &systemPalette);
    resources.palettes[QPlatformTheme::SystemPalette] = new QPalette(systemPalette);
}

// Reads the color from the color scheme, and store it in the
// palette with the given color role if found.
bool HawaiiThemePrivate::readColor(QPalette *pal, QPalette::ColorRole role, const QVariant &value)
{
    if (!value.isValid())
        return false;
    const QStringList values = value.toStringList();
    if (values.size() != 3)
        return false;
    pal->setBrush(role, QColor(values.at(0).toInt(), values.at(1).toInt(), values.at(2).toInt()));
    return true;
}
