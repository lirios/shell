/****************************************************************************
 * This file is part of Hawaii.
 *
 * Copyright (C) 2010-2015 Pier Luigi Fiorini
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

#include <QtCore/QEvent>
#include <QtCore/QFileInfo>
#include <QtCore/QVariant>
#include <QtCore/QStandardPaths>
#include <QtGui/QFont>
#include <QtGui/QPalette>
#include <QtGui/QTextCharFormat>

#include "hawaiitheme.h"
#include "hawaiitheme_p.h"

ResourceHelper::ResourceHelper()
{
    std::fill(palettes, palettes + QPlatformTheme::NPalettes, static_cast<QPalette *>(0));
    std::fill(fonts, fonts + QPlatformTheme::NFonts, static_cast<QFont *>(0));
}

void ResourceHelper::clear()
{
    qDeleteAll(palettes, palettes + QPlatformTheme::NPalettes);
    std::fill(palettes, palettes + QPlatformTheme::NPalettes, static_cast<QPalette *>(0));
    qDeleteAll(fonts, fonts + QPlatformTheme::NFonts);
    std::fill(fonts, fonts + QPlatformTheme::NFonts, static_cast<QFont *>(0));
}


HawaiiTheme::HawaiiTheme()
    : QPlatformTheme(new HawaiiThemePrivate())
{
    Q_D(HawaiiTheme);
    d->refresh();
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
    Q_D(const HawaiiTheme);
    return d->resources.palettes[type];
}

const QFont *HawaiiTheme::font(Font type) const
{
    Q_D(const HawaiiTheme);
    return d->resources.fonts[type];
}

QVariant HawaiiTheme::themeHint(ThemeHint hint) const
{
    Q_D(const HawaiiTheme);

    switch (hint) {
    case DropShadow:
        return QVariant(true);
    case ToolButtonStyle: {
        QString val = d->uiSettings->value(QStringLiteral("tool-button-style")).toString();

        if (val == QStringLiteral("icon-only"))
            return QVariant(int(Qt::ToolButtonIconOnly));
        else if (val == QStringLiteral("text-only"))
            return QVariant(int(Qt::ToolButtonTextOnly));
        else if (val == QStringLiteral("text-beside-icon"))
            return QVariant(int(Qt::ToolButtonTextBesideIcon));
        else if (val == QStringLiteral("text-under-icon"))
            return QVariant(int(Qt::ToolButtonTextUnderIcon));

        return QVariant(int(Qt::ToolButtonFollowStyle));
    }
    case ToolBarIconSize: {
        QString val = d->uiSettings->value(QStringLiteral("toolbar-icons-size")).toString();

        if (val == QStringLiteral("small"))
            return 24;
        return 48;
    }
    case ItemViewActivateItemOnSingleClick:
        return QVariant(false);
    case SystemIconThemeName:
        return d->uiSettings->value(QStringLiteral("icon-theme")).toString();
    case SystemIconFallbackThemeName:
        return QStringLiteral("hicolor");
    case IconThemeSearchPaths:
        return QVariant(QStandardPaths::locateAll(
                            QStandardPaths::GenericDataLocation,
                            QStringLiteral("icons"),
                            QStandardPaths::LocateDirectory));
    case StyleNames: {
        QStringList styles;
        styles << d->uiSettings->value(QStringLiteral("widgets-style")).toString();
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
