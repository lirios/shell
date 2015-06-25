/****************************************************************************
 * This file is part of Hawaii.
 *
 * Copyright (C) 2015 Pier Luigi Fiorini
 *
 * Author(s):
 *    Pier Luigi Fiorini <pierluigi.fiorini@gmail.com>
 *
 * $BEGIN_LICENSE:GPL3-HAWAII$
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; version 3 or any later version accepted
 * by Pier Luigi Fiorini, which shall act as a proxy defined in Section 14
 * of version 3 of the license.
 *
 * Any modifications to this file must keep this entire header intact.
 *
 * The interactive user interfaces in modified source and object code
 * versions of this program must display Appropriate Legal Notices,
 * as required under Section 5 of the GNU General Public License version 3.
 *
 * In accordance with Section 7(b) of the GNU General Public License
 * version 3, these Appropriate Legal Notices must retain the display of the
 * "Powered by Hawaii" logo.  If the display of the logo is not reasonably
 * feasible for technical reasons, the Appropriate Legal Notices must display
 * the words "Powered by Hawaii".
 *
 * In accordance with Section 7(c) of the GNU General Public License
 * version 3, modified source and object code versions of this program
 * must be marked in reasonable ways as different from the original version.
 *
 * In accordance with Section 7(d) of the GNU General Public License
 * version 3, neither the "Hawaii" name, nor the name of any project that is
 * related to it, nor the names of its contributors may be used to endorse or
 * promote products derived from this software without specific prior written
 * permission.
 *
 * In accordance with Section 7(e) of the GNU General Public License
 * version 3, this license does not grant any license or rights to use the
 * "Hawaii" name or logo, nor any other trademark.
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
    settings = new Hawaii::QGSettings(QStringLiteral("org.hawaii.desktop.interface"),
                                      QStringLiteral("/org/hawaii/desktop/interface/"));
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
