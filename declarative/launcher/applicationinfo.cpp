/****************************************************************************
* This file is part of Hawaii.
 *
 * Copyright (C) 2015-2016 Pier Luigi Fiorini
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

#include <QtCore/QLocale>

#include "appidmapping_p.h"
#include "applicationaction.h"
#include "applicationinfo.h"
#include "applicationinfo_p.h"

/*
 * ApplicationInfoPrivate
 */

ApplicationInfoPrivate::ApplicationInfoPrivate(const QString &origAppId, ApplicationInfo *parent)
    : state(ApplicationInfo::NotRunning)
    , entry(Q_NULLPTR)
    , focused(false)
    , q_ptr(parent)
{
    appId = AppIdMapping::mapAppId(origAppId);
    fileName = AppIdMapping::desktopFileName(appId);

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

QString ApplicationInfo::fileName() const
{
    Q_D(const ApplicationInfo);

    if (d->entry)
        return d->entry->fileName();
    return QString();
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
