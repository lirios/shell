/****************************************************************************
 * This file is part of Vibe.
 *
 * Copyright (c) 2010-2012 Pier Luigi Fiorini
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

#include <QFileInfo>
#include <QEvent>
#include <QDesktopServices>
#include <QFont>
#include <QVariant>
#include <QStandardPaths>
#include <QTextCharFormat>

#include <VibeCore/VSettings>
#include <VibeGui/VColorScheme>

#include "hawaiitheme.h"

HawaiiTheme::HawaiiTheme()
{
    m_settings = new VSettings("org.hawaii.desktop");
}

HawaiiTheme::~HawaiiTheme()
{
    delete m_settings;
}

bool HawaiiTheme::usePlatformNativeDialog(DialogType type) const
{
    return false;
}

QPlatformDialogHelper *HawaiiTheme::createPlatformDialogHelper(DialogType type) const
{
    return 0;
}

const QPalette *HawaiiTheme::palette(Palette type) const
{
#if 0
    switch (type) {
        case SystemPalette: {
            QString colorSchemeName = m_settings->value("interface/color-scheme").toString();
            if (colorSchemeName.isEmpty())
                return QPlatformTheme::palette(type);

            VColorScheme colorScheme(QString("%1/%2.colors").arg(path).arg(colorSchemeName));
            return colorScheme.palette();
        }
        default:
            break;
    }
#endif

    return new QPalette();
}

const QFont *HawaiiTheme::font(Font type) const
{
    QString fontNameKey = QLatin1String("interface/font-name");
    QString fontSizeKey = QLatin1String("interface/font-size");

    switch (type) {
        case SmallFont:
            fontNameKey = QLatin1String("interface/small-font-name");
            fontSizeKey = QLatin1String("interface/small-font-size");
            break;
        case MiniFont:
            fontNameKey = QLatin1String("interface/mini-font-name");
            fontSizeKey = QLatin1String("interface/mini-font-size");
            break;
        default:
            break;
    }

    QString fontName = m_settings->value(fontNameKey).toString();
    int fontSize = m_settings->value(fontSizeKey).toInt();
    QFont *font = new QFont(fontName, fontSize);

    switch (type) {
        case TitleBarFont:
        case MdiSubWindowTitleFont:
        case DockWidgetTitleFont:
            font->setBold(true);
            break;
        default:
            break;
    }

    return font;
}

QVariant HawaiiTheme::themeHint(ThemeHint hint) const
{
    switch (hint) {
        case DropShadow:
            return QVariant(true);
        case ToolButtonStyle: {
            QString val = m_settings->value("interface/toolbutton-style").toString();

            if (val == "icon-only")
                return QVariant(int(Qt::ToolButtonIconOnly));
            else if (val == "text-only")
                return QVariant(int(Qt::ToolButtonTextOnly));
            else if (val == "text-beside-icon")
                return QVariant(int(Qt::ToolButtonTextBesideIcon));
            else if (val == "text-under-icon")
                return QVariant(int(Qt::ToolButtonTextUnderIcon));

            return QVariant(int(Qt::ToolButtonFollowStyle));
        }
        case ToolBarIconSize:
            return m_settings->value("interface/toolbar-icon-size");
        case ItemViewActivateItemOnSingleClick:
            return QVariant(false);
        case SystemIconThemeName:
            return m_settings->value("interface/icon-theme");
        case SystemIconFallbackThemeName:
            return QVariant("hicolor");
        case IconThemeSearchPaths:
            return QVariant(QStandardPaths::locateAll(
                        QStandardPaths::GenericDataLocation,
                        "icons", QStandardPaths::LocateDirectory));
        case StyleNames: {
            QStringList styles;
            styles << m_settings->value("interface/style").toString();
            return QVariant(styles);
        }
        case WindowAutoPlacement:
            return QVariant(true);
        case DialogButtonBoxLayout:
            return QVariant(1); // QDialogButtonBox::MacLayout
        case DialogButtonBoxButtonsHaveIcons:
            return QVariant(false);
        case UseFullScreenForPopupMenu:
            return QVariant(true);
        case KeyboardScheme:
             // TODO: Use the Mac keyboard scheme only if an Apple keyboard is detected
            //return QVariant(int(MacKeyboardScheme));
            return QVariant(int(GnomeKeyboardScheme));
        case UiEffects:
            return AnimateMenuUiEffect | FadeMenuUiEffect |
                   AnimateComboUiEffect | AnimateTooltipUiEffect |
                   FadeTooltipUiEffect | AnimateToolBoxUiEffect;
        case SpellCheckUnderlineStyle:
            return QVariant(int(QTextCharFormat::SpellCheckUnderline));
        case TabAllWidgets:
            return QVariant(true);
        case IconPixmapSizes: {
            QList<int> list;
            list << 16 << 22 << 24 << 32 << 48 << 64 << 128 << 256;
            return QVariant::fromValue(list);
        }
        default:
            break;
    }

    return QPlatformTheme::themeHint(hint);
}
