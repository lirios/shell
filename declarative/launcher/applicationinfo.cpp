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
    }
}

ApplicationInfoPrivate::~ApplicationInfoPrivate()
{
    while (!actions.isEmpty())
        actions.takeFirst()->deleteLater();
    entry->deleteLater();
}

QVariant ApplicationInfoPrivate::value(const QString &group, const QString &key, const QVariant &defaultValue) const
{
    if (!entry)
        return defaultValue;

    entry->beginGroup(group);
    QVariant value = entry->value(key, defaultValue);
    entry->endGroup();

    return value;
}

QVariant ApplicationInfoPrivate::localizedValue(const QString &group, const QString &key, const QVariant &defaultValue) const
{
    if (!entry)
        return defaultValue;

    QVariant value = defaultValue;

    QString locale = QLocale().name();
    QString shortLocale = locale.split('_').first();

    const QString longKey = QStringLiteral("%1[%2]").arg(key).arg(locale);
    const QString shortKey = QStringLiteral("%1[%2]").arg(key).arg(shortLocale);

    entry->beginGroup(group);

    // First try with Key[xx_YY], then Key[XX] and fall
    // back to just Key
    if (locale != shortLocale && entry->contains(longKey))
        value = entry->value(longKey, defaultValue);
    else if (entry->contains(shortKey))
        value = entry->value(shortKey, defaultValue);
    else
        value = entry->value(key, defaultValue);

    entry->endGroup();

    return value;
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
    if (!entry)
        return;

    Q_Q(ApplicationInfo);

    QStringList actionNames = value(QLatin1String("Desktop Entry"),
                                    QLatin1String("Actions")).toString().split(';', QString::SkipEmptyParts);
    Q_FOREACH (const QString &actionName, actionNames) {
        const QString group = QStringLiteral("Desktop Action %1").arg(actionName);

        // Name is mandatory, the specification says to ignore actions without it
        if (localizedValue(group, QLatin1String("Name")).toString().isEmpty())
            continue;

        ApplicationAction *action = new ApplicationAction(
                    localizedValue(group, QLatin1String("Name")).toString(),
                    localizedValue(group, QLatin1String("Icon")).toString(),
                    localizedValue(group, QLatin1String("Exec")).toString(),
                    q);
        actions.append(action);
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
    return d->localizedValue(QLatin1String("Desktop Entry"),
                             QLatin1String("Name")).toString();
}

QString ApplicationInfo::comment() const
{
    Q_D(const ApplicationInfo);
    return d->localizedValue(QLatin1String("Desktop Entry"),
                             QLatin1String("Comment")).toString();
}

QString ApplicationInfo::iconName() const
{
    Q_D(const ApplicationInfo);
    return d->value(QLatin1String("Desktop Entry"),
                    QLatin1String("Icon"),
                    QLatin1String("application-octet-stream")).toString();
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
