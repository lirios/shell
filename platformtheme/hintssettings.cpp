/****************************************************************************
 * This file is part of Hawaii.
 *
 * Copyright (C) 2010-2016 Pier Luigi Fiorini
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
#include <QtGui/QTextCharFormat>
#include <QtWidgets/QApplication>
#include <QtWidgets/QMainWindow>
#include <QtWidgets/QWidget>
#include <QtWidgets/QToolBar>
#include <QtWidgets/QToolButton>

#include <Hawaii/QGSettings>

#include "hintssettings.h"

HintsSettings::HintsSettings(Hawaii::QGSettings *settings, QObject *parent)
    : QObject(parent)
    , m_settings(settings)
{
    // Change only the few hints involved, for some of these settings
    // we need to take actions to refresh applications
    connect(m_settings, &Hawaii::QGSettings::settingChanged, this,
            [this](const QString &key) {
        if (key == QStringLiteral("cursorBlinkTime"))
            qtSettingsChanged();
        else if (key == QStringLiteral("toolButtonStyle"))
            toolButtonStyleChanged();
        else if (key == QStringLiteral("iconTheme") ||
                 key == QStringLiteral("toolbarIconsSize"))
            iconChanged();
        else if (key == QStringLiteral("widgetsStyle"))
            styleChanged();
    });
}

void HintsSettings::collectHints()
{
    m_hints.clear();

    m_hints.insert(QPlatformTheme::CursorFlashTime,
                   m_settings->value(QStringLiteral("cursorBlinkTime")));
    m_hints.insert(QPlatformTheme::DropShadow, true);
    m_hints.insert(QPlatformTheme::ToolButtonStyle,
                   toolButtonStyle(m_settings->value(QStringLiteral("toolButtonStyle"))));
    m_hints.insert(QPlatformTheme::ToolBarIconSize,
                   toolBarIconSize(m_settings->value(QStringLiteral("toolbarIconsSize"))));
    m_hints.insert(QPlatformTheme::ItemViewActivateItemOnSingleClick, false);
    m_hints.insert(QPlatformTheme::SystemIconThemeName,
                   m_settings->value(QStringLiteral("iconTheme")));
    m_hints.insert(QPlatformTheme::SystemIconFallbackThemeName, QStringLiteral("hicolor"));
    m_hints.insert(QPlatformTheme::IconThemeSearchPaths,
                   QStandardPaths::locateAll(
                       QStandardPaths::GenericDataLocation,
                       QStringLiteral("icons"),
                       QStandardPaths::LocateDirectory));
    m_hints.insert(QPlatformTheme::StyleNames,
                   QStringList() << m_settings->value(QStringLiteral("widgetsStyle")).toString());
    m_hints.insert(QPlatformTheme::WindowAutoPlacement, true);
    m_hints.insert(QPlatformTheme::DialogButtonBoxLayout, 1); // QDialogButtonBox::MacLayout
    m_hints.insert(QPlatformTheme::DialogButtonBoxButtonsHaveIcons, false);
    m_hints.insert(QPlatformTheme::UseFullScreenForPopupMenu, true);
    // TODO: Use the Mac keyboard scheme only if an Apple keyboard is detected
    // int(MacKeyboardScheme);
    m_hints.insert(QPlatformTheme::KeyboardScheme,
                   int(QPlatformTheme::GnomeKeyboardScheme));
    m_hints.insert(QPlatformTheme::UiEffects,
                   QPlatformTheme::AnimateMenuUiEffect |
                   QPlatformTheme::FadeMenuUiEffect |
                   QPlatformTheme::AnimateComboUiEffect |
                   QPlatformTheme::AnimateTooltipUiEffect |
                   QPlatformTheme::FadeTooltipUiEffect |
                   QPlatformTheme::AnimateToolBoxUiEffect);
    m_hints.insert(QPlatformTheme::SpellCheckUnderlineStyle,
                   int(QTextCharFormat::SpellCheckUnderline));
    m_hints.insert(QPlatformTheme::TabAllWidgets, true);
#if QT_VERSION >= QT_VERSION_CHECK(5, 5, 0)
    m_hints.insert(QPlatformTheme::TabFocusBehavior, int(Qt::TabFocusAllControls));
#endif
    QList<int> pixmapSizes;
    pixmapSizes
            << 512 << 256 << 128 << 64 << 48
            << 32 << 24 << 22 << 16;
    m_hints.insert(QPlatformTheme::IconPixmapSizes, QVariant::fromValue(pixmapSizes));
}

Qt::ToolButtonStyle HintsSettings::toolButtonStyle(const QVariant &value)
{
    const QString style = value.toString();

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

int HintsSettings::toolBarIconSize(const QVariant &value)
{
    const QString size = value.toString();

    if (size == QStringLiteral("small"))
        return 24;
    return 48;
}

void HintsSettings::qtSettingsChanged()
{
    // Change the hints
    m_hints.insert(QPlatformTheme::CursorFlashTime,
                   m_settings->value(QStringLiteral("cursorBlinkTime")));
}

void HintsSettings::toolButtonStyleChanged()
{
    // Change the hint
    m_hints.insert(QPlatformTheme::ToolButtonStyle,
                   toolButtonStyle(m_settings->value(QStringLiteral("toolButtonStyle"))));

    // Continue only for QWidgets applications
    if (!qobject_cast<QApplication *>(QCoreApplication::instance()))
        return;

    // Refresh all tool button widgets
    QWidgetList widgets = QApplication::allWidgets();
    Q_FOREACH (QWidget *widget, widgets) {
        if (qobject_cast<QToolButton *>(widget)) {
            QEvent event(QEvent::StyleChange);
            QApplication::sendEvent(widget, &event);
        }
    }
}

void HintsSettings::iconChanged()
{
    // Change the hint
    m_hints.insert(QPlatformTheme::SystemIconThemeName,
                   m_settings->value(QStringLiteral("iconTheme")));

    // This method might be called when only the icon theme has changed,
    // in that case we don't refresh the style
    int newSize = toolBarIconSize(m_settings->value(QStringLiteral("toolbarIconsSize")));
    if (m_hints[QPlatformTheme::ToolBarIconSize] != newSize) {
        m_hints.insert(QPlatformTheme::ToolBarIconSize, newSize);

        // Continue only for QWidgets applications
        if (!qobject_cast<QApplication *>(QCoreApplication::instance()))
            return;

        // Refresh the main window and all tool bars
        QWidgetList widgets = QApplication::allWidgets();
        Q_FOREACH (QWidget *widget, widgets) {
            if (qobject_cast<QToolBar *>(widget) || qobject_cast<QMainWindow *>(widget)) {
                QEvent event(QEvent::StyleChange);
                QApplication::sendEvent(widget, &event);
            }
        }
    }
}

void HintsSettings::styleChanged()
{
    // Change the hint
    m_hints.insert(QPlatformTheme::StyleNames,
                   QStringList() << m_settings->value(QStringLiteral("widgetsStyle")).toString());

    // Continue only for QWidgets applications
    if (!qobject_cast<QApplication *>(QCoreApplication::instance()))
        return;

    // Change the style
    const QString style = m_hints[QPlatformTheme::StyleNames].toString();
    if (!style.isEmpty())
        QApplication::setStyle(style);
    // TODO: load pal
}

#include "moc_hintssettings.cpp"
