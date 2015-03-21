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
#include <QtCore/QSettings>
#include <QtCore/QStandardPaths>
#include <QtGui/QFont>
#include <QtGui/QPalette>
#include <QtGui/QTextCharFormat>

#include "hawaiitheme.h"
#include "hawaiitheme_p.h"

ResourceHelper::ResourceHelper()
{
    std::fill(palettes, palettes + QPlatformTheme::NPalettes, static_cast<QPalette *>(0));
}

void ResourceHelper::clear()
{
    qDeleteAll(palettes, palettes + QPlatformTheme::NPalettes);
    std::fill(palettes, palettes + QPlatformTheme::NPalettes, static_cast<QPalette *>(0));
}


HawaiiTheme::HawaiiTheme()
    : QPlatformTheme(new HawaiiThemePrivate())
{
    m_settings = new QSettings("Hawaii", "Desktop");
    d_func()->refresh();
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
    Q_D(const HawaiiTheme);
    return d->resources.palettes[type];
}

const QFont *HawaiiTheme::font(Font type) const
{
    QString fontName = QStringLiteral("Droid Sans");
    int fontSize = 11;

    switch (type) {
    case SystemFont:
        fontName = m_settings->value("interface/font-name", fontName).toString();
        fontSize = m_settings->value("interface/font-size", fontSize).toInt();
        break;
    case SmallFont:
        fontName = m_settings->value("interface/small-font-name", fontName).toString();
        fontSize = m_settings->value("interface/small-font-size", fontSize).toInt();
        break;
    case MiniFont:
        fontName = m_settings->value("interface/mini-font-name", fontName).toString();
        fontSize = m_settings->value("interface/mini-font-size", fontSize).toInt();
        break;
    default:
        break;
    }

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
        QString val = m_settings->value("interface/toolbutton-style", "icon-only").toString();

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
        return m_settings->value("interface/toolbar-icon-size", 24);
    case ItemViewActivateItemOnSingleClick:
        return QVariant(false);
    case SystemIconThemeName:
        return m_settings->value("interface/icon-theme", "elegant");
    case SystemIconFallbackThemeName:
        return QVariant("hicolor");
    case IconThemeSearchPaths:
        return QVariant(QStandardPaths::locateAll(
                            QStandardPaths::GenericDataLocation,
                            "icons", QStandardPaths::LocateDirectory));
    case StyleNames: {
        QStringList styles;
        styles << m_settings->value("interface/style", "Fusion").toString();
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
