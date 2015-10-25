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
#include <QtGui/QFont>
#include <QtGui/QPalette>
#include <qpa/qplatformtheme.h>

#include "hawaiitheme_p.h"
#include "hintssettings.h"

// Default fonts
static const char defaultSystemFontName[] = "Droid Sans";
static int defaultSystemFontSize = 11;

HawaiiThemePrivate::HawaiiThemePrivate()
{
    settings = new Hawaii::QGSettings(QStringLiteral("org.hawaiios.desktop.interface"),
                                      QStringLiteral("/org/hawaiios/desktop/interface/"));
    hints = new HintsSettings(settings);
}

HawaiiThemePrivate::~HawaiiThemePrivate()
{
    hints->deleteLater();
    settings->deleteLater();
}

static QFont *readFont(const QString &family, int size)
{
    QFont font(family, size);
    return new QFont(font);
}

void HawaiiThemePrivate::refresh()
{
    resources.clear();

    // Load hints
    hints->collectHints();

    // Locate color scheme
    QString scheme = settings->value(QStringLiteral("colorScheme")).toString();
    QString schemeFileName = QStandardPaths::locate(QStandardPaths::GenericDataLocation,
                                                    QString("color-schemes/%1.colors").arg(scheme));

    // Palette
    QPalette systemPalette = QPalette();
    ResourceHelper::readPalette(schemeFileName, &systemPalette);
    resources.palettes[QPlatformTheme::SystemPalette] = new QPalette(systemPalette);

    // Fonts
    QString fontFamily = settings->value(QStringLiteral("fontName")).toString();
    int fontSize = settings->value(QStringLiteral("fontSize")).toInt();
    QString smallFontFamily = settings->value(QStringLiteral("smallFontName")).toString();
    int smallFontSize = settings->value(QStringLiteral("smallFontSize")).toInt();
    QString miniFontFamily = settings->value(QStringLiteral("miniFontName")).toString();
    int miniFontSize = settings->value(QStringLiteral("miniFontSize")).toInt();

    // System font
    if (QFont *systemFont = readFont(fontFamily, fontSize))
        resources.fonts[QPlatformTheme::SystemFont] = systemFont;
    else
        resources.fonts[QPlatformTheme::SystemFont] = new QFont(QLatin1String(defaultSystemFontName), defaultSystemFontSize);

    // Small font
    if (QFont *smallFont = readFont(smallFontFamily, smallFontSize))
        resources.fonts[QPlatformTheme::SmallFont] = smallFont;
    else
        resources.fonts[QPlatformTheme::SmallFont] = new QFont(QLatin1String(defaultSystemFontName), defaultSystemFontSize);

    // Mini font
    if (QFont *miniFont = readFont(miniFontFamily, miniFontSize))
        resources.fonts[QPlatformTheme::MiniFont] = miniFont;
    else
        resources.fonts[QPlatformTheme::MiniFont] = new QFont(QLatin1String(defaultSystemFontName), defaultSystemFontSize);

    // Other fonts
    QList<QPlatformTheme::Font> fonts;
    fonts << QPlatformTheme::TitleBarFont << QPlatformTheme::MdiSubWindowTitleFont
          << QPlatformTheme::DockWidgetTitleFont;
    Q_FOREACH (QPlatformTheme::Font font, fonts) {
        if (QFont *systemFont = readFont(fontFamily, fontSize))
            resources.fonts[font] = systemFont;
        else
            resources.fonts[font] = new QFont(QLatin1String(defaultSystemFontName), defaultSystemFontSize);
        resources.fonts[font]->setBold(true);
    }
}
