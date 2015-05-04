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
    collectHints();
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

Qt::ToolButtonStyle HawaiiTheme::toolButtonStyle(const QString &style)
{
    if (style == QStringLiteral("icon-only"))
        return Qt::ToolButtonIconOnly;
    else if (style == QStringLiteral("text-only"))
        return Qt::ToolButtonTextOnly;
    else if (style == QStringLiteral("text-beside-icon"))
        return Qt::ToolButtonTextBesideIcon;
    else if (style == QStringLiteral("text-under-icon"))
        return Qt::ToolButtonTextUnderIcon;

    return Qt::ToolButtonFollowStyle;
}

int HawaiiTheme::toolBarIconSize(const QString &size)
{
    if (size == QStringLiteral("small"))
        return 24;
    return 48;
}

void HawaiiTheme::collectHints()
{
    Q_D(HawaiiTheme);

    m_hints.clear();

    m_hints.insert(CursorFlashTime,
                   d->settings->value(QStringLiteral("cursorBlinkTime")));
    m_hints.insert(DropShadow, true);
    m_hints.insert(ToolButtonStyle, toolButtonStyle(
                       d->settings->value(
                           QStringLiteral("toolButtonStyle")).toString()));
    m_hints.insert(ToolBarIconSize, toolBarIconSize(
                       d->settings->value(
                           QStringLiteral("toolbarIconsSize")).toString()));
    m_hints.insert(ItemViewActivateItemOnSingleClick, false);
    m_hints.insert(SystemIconThemeName, d->settings->value(QStringLiteral("iconTheme")));
    m_hints.insert(SystemIconFallbackThemeName, QStringLiteral("hicolor"));
    m_hints.insert(IconThemeSearchPaths,
                   QStandardPaths::locateAll(
                       QStandardPaths::GenericDataLocation,
                       QStringLiteral("icons"),
                       QStandardPaths::LocateDirectory));
    m_hints.insert(StyleNames,
                   QStringList() << d->settings->value(QStringLiteral("widgetsStyle")).toString());
    m_hints.insert(WindowAutoPlacement, true);
    m_hints.insert(DialogButtonBoxLayout, 1); // QDialogButtonBox::MacLayout
    m_hints.insert(DialogButtonBoxButtonsHaveIcons, false);
    m_hints.insert(UseFullScreenForPopupMenu, true);
    // TODO: Use the Mac keyboard scheme only if an Apple keyboard is detected
    // int(MacKeyboardScheme);
    m_hints.insert(KeyboardScheme, int(GnomeKeyboardScheme));
    m_hints.insert(UiEffects,
                   AnimateMenuUiEffect | FadeMenuUiEffect |
                   AnimateComboUiEffect | AnimateTooltipUiEffect |
                   FadeTooltipUiEffect | AnimateToolBoxUiEffect);
    m_hints.insert(SpellCheckUnderlineStyle,
                   int(QTextCharFormat::SpellCheckUnderline));
    m_hints.insert(TabAllWidgets, true);
    QList<int> pixmapSizes;
    pixmapSizes
            << 512 << 256 << 128 << 64 << 48
            << 32 << 24 << 22 << 16;
    m_hints.insert(IconPixmapSizes, QVariant::fromValue(pixmapSizes));

    // Default hints
    QList<QPlatformTheme::ThemeHint> hints;
    hints
            << KeyboardInputInterval
            << MouseDoubleClickInterval
            << StartDragDistance
            << StartDragTime
            << KeyboardAutoRepeatRate
            << PasswordMaskDelay
            << StartDragVelocity
            << TextCursorWidth
            << MaximumScrollBarDragDistance
            << TabFocusBehavior
            << PasswordMaskCharacter
            << DialogSnapToDefaultButton
            << ContextMenuOnMouseRelease
            << MousePressAndHoldInterval
            << MouseDoubleClickDistance
            << WheelScrollLines;
    Q_FOREACH (const QPlatformTheme::ThemeHint hint, hints)
        m_hints.insert(hint, QPlatformTheme::themeHint(hint));
}
