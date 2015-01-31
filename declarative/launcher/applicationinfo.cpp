/****************************************************************************
* This file is part of Hawaii.
 *
 * Copyright (C) 2015 Pier Luigi Fiorini <pierluigi.fiorini@gmail.com>
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

#include <QtCore/QLocale>
#include <QtCore/QStandardPaths>
#include <QtCore/QDebug>

#include "applicationaction.h"
#include "applicationinfo.h"
#include "applicationinfo_p.h"

/*
 * ApplicationInfoPrivate
 */

ApplicationInfoPrivate::ApplicationInfoPrivate(const QString &appId, ApplicationInfo *parent)
    : appId(appId)
    , state(ApplicationInfo::NotRunning)
    , entry(Q_NULLPTR)
    , focused(false)
    , q_ptr(parent)
{
    // Replace '-' with '/' as mentioned here:
    // http://standards.freedesktop.org/desktop-entry-spec/1.1/ape.html
    const QString name = QString(appId).replace('-', '/');

    // Now the fun part begins: Qt sets app_id to program name + ".desktop",
    // while GTK+ uses the program name starting with an upper case letter.
    // It seems a pattern with D-Bus activatable it goes like this: let's take
    // GNOME Clocks as an example: Gnome-clocks app_id and
    // org.gnome.clocks.desktop desktop entry.

    // Case 1: assume app_id is already the desktop file base name (Qt)
    fileName = QStandardPaths::locate(QStandardPaths::ApplicationsLocation,
                                      name);

    // Case 2: try without changing the case appending ".desktop"
    if (fileName.isEmpty()) {
        fileName = QStandardPaths::locate(QStandardPaths::ApplicationsLocation,
                                          name + QStringLiteral(".desktop"));
    }

    // Case 3: lower case and append ".desktop"
    if (fileName.isEmpty()) {
        fileName = QStandardPaths::locate(QStandardPaths::ApplicationsLocation,
                                          name.toLower() + QStringLiteral(".desktop"));
    }

    // Case 4: lower case, do not replace '-' with '/' and append ".desktop" (GTK+)
    if (fileName.isEmpty())
        fileName = QStandardPaths::locate(QStandardPaths::ApplicationsLocation,
                                          appId.toLower() + QStringLiteral(".desktop"));

    // Case 5: D-Bus activatable applications: lower case appId, split with '-'
    // and treat the first as a reverse domain name prepending org (yeah it might
    // be another extension...)
    QStringList pieces = appId.toLower().split('-', QString::SkipEmptyParts);
    if (pieces.size() == 2) {
        QString processedAppId = QStringLiteral("org.%1.%2")
                .arg(pieces.at(0)).arg(pieces.at(1));
        if (fileName.isEmpty())
            fileName = QStandardPaths::locate(QStandardPaths::ApplicationsLocation,
                                              processedAppId + QStringLiteral(".desktop"));
    }

    if (!fileName.isEmpty()) {
        // Open the desktop file
        entry = new QSettings(fileName, QSettings::IniFormat);
        entry->setIniCodec("UTF-8");

        // Actions
        retrieveActions();

        // Read from the main section from now on
        entry->beginGroup(QStringLiteral("Desktop Entry"));
    }
}

ApplicationInfoPrivate::~ApplicationInfoPrivate()
{
    while (!actions.isEmpty())
        actions.takeFirst()->deleteLater();
    entry->deleteLater();
}

QVariant ApplicationInfoPrivate::value(const QString &key, const QVariant &defaultValue) const
{
    if (!entry)
        return defaultValue;
    return entry->value(key, defaultValue);
}

QVariant ApplicationInfoPrivate::localizedValue(const QString &key, const QVariant &defaultValue) const
{
    if (!entry)
        return defaultValue;

    QString locale = QLocale().name();
    QString shortLocale = locale.split('_').first();

    const QString longKey = QStringLiteral("%1[%2]").arg(key).arg(locale);
    const QString shortKey = QStringLiteral("%1[%2]").arg(key).arg(shortLocale);

    // First try with Key[xx_YY], then Key[XX] and fall
    // back to just Key
    if (locale != shortLocale && entry->contains(longKey))
        return entry->value(longKey, defaultValue);
    if (entry->contains(shortKey))
        return entry->value(shortKey, defaultValue);
    return entry->value(key, defaultValue);
}

void ApplicationInfoPrivate::setState(ApplicationInfo::State value)
{
    Q_Q(ApplicationInfo);

    if (state == value)
        return;

    state = value;
    Q_EMIT q->stateChanged();
}

void ApplicationInfoPrivate::retrieveActions()
{
    if (!entry || !entry->contains(QStringLiteral("Actions")))
        return;

    Q_Q(ApplicationInfo);

    QStringList actionNames = entry->value(QStringLiteral("Actions")).toString().split(';', QString::SkipEmptyParts);
    for (const QString &actionName: actionNames) {
        entry->beginGroup(QStringLiteral("Desktop Action %1").arg(actionName));

        // Name is mandatory, the specification says to ignore actions without it
        if (!entry->childKeys().contains(QStringLiteral("Name")))
            continue;

        ApplicationAction *action = new ApplicationAction(
                    localizedValue(QStringLiteral("Name")).toString(),
                    localizedValue(QStringLiteral("Icon")).toString(),
                    localizedValue(QStringLiteral("Exec")).toString(),
                    q);
        actions.append(action);

        entry->endGroup();
    }
}

/*
 * ApplicationInfo
 */

ApplicationInfo::ApplicationInfo(const QString &appId, QObject *parent)
    : QObject(parent)
    , d_ptr(new ApplicationInfoPrivate(appId, this))
{
    qRegisterMetaType<ApplicationInfo *>("ApplicationInfo*");
}

ApplicationInfo::~ApplicationInfo()
{
    delete d_ptr;
}

ApplicationInfo::State ApplicationInfo::state() const
{
    Q_D(const ApplicationInfo);
    return d->state;
}

QString ApplicationInfo::appId() const
{
    Q_D(const ApplicationInfo);
    return d->appId;
}

QString ApplicationInfo::name() const
{
    Q_D(const ApplicationInfo);
    return d->localizedValue(QStringLiteral("Name")).toString();
}

QString ApplicationInfo::comment() const
{
    Q_D(const ApplicationInfo);
    return d->localizedValue(QStringLiteral("Comment")).toString();
}

QString ApplicationInfo::iconName() const
{
    Q_D(const ApplicationInfo);
    return d->value(QStringLiteral("Icon"),
                    QStringLiteral("application-octet-stream")).toString();
}

bool ApplicationInfo::isFocused() const
{
    Q_D(const ApplicationInfo);
    return d->focused;
}

QList<ApplicationAction *> ApplicationInfo::actions() const
{
    Q_D(const ApplicationInfo);
    return d->actions;
}

#include "moc_applicationinfo.cpp"
