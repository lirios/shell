/****************************************************************************
 * This file is part of Hawaii Shell.
 *
 * Copyright (C) 2012-2014 Pier Luigi Fiorini <pierluigi.fiorini@gmail.com>
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

#include <QtConfiguration/QConfiguration>

#include "shellsettings.h"
#include "shellsettings_p.h"

/*
 * ShellSettingsPrivate
 */

ShellSettingsPrivate::ShellSettingsPrivate()
{
}

/*
 * ShellSettings
 */

ShellSettings::ShellSettings(QObject *parent)
    : QObject(parent)
    , d_ptr(new ShellSettingsPrivate())
{
    Q_D(ShellSettings);
    d->style = QStringLiteral("Aluminium");
    d->handler = QStringLiteral("org.hawaii.shells.desktop");
    d->lookAndFeel = QStringLiteral("org.hawaii.lookandfeel.standard");
    d->background = QStringLiteral("org.hawaii.backgrounds.wallpaper");
    d->configuration = new QConfiguration(this, "shell");
}

ShellSettings::~ShellSettings()
{
    delete d_ptr;
}

QString ShellSettings::style() const
{
    Q_D(const ShellSettings);
    return d->style;
}

void ShellSettings::setStyle(const QString &style)
{
    Q_D(ShellSettings);

    if (style != d->style) {
        d->style = style;
        Q_EMIT styleChanged();
    }
}

QString ShellSettings::handler() const
{
    Q_D(const ShellSettings);
    return d->handler;
}

void ShellSettings::setHandler(const QString &handler)
{
    Q_D(ShellSettings);

    if (handler != d->handler) {
        d->handler = handler;
        Q_EMIT handlerChanged();
    }
}

QString ShellSettings::lookAndFeel() const
{
    Q_D(const ShellSettings);
    return d->lookAndFeel;
}

void ShellSettings::setLookAndFeel(const QString &lookAndFeel)
{
    Q_D(ShellSettings);

    if (lookAndFeel != d->lookAndFeel) {
        d->lookAndFeel = lookAndFeel;
        Q_EMIT lookAndFeelChanged();
    }
}

QString ShellSettings::background() const
{
    Q_D(const ShellSettings);
    return d->background;
}

void ShellSettings::setBackground(const QString &plugin)
{
    Q_D(ShellSettings);

    if (plugin != d->background) {
        d->background = plugin;
        Q_EMIT backgroundChanged();
    }
}

#include "moc_shellsettings.cpp"
