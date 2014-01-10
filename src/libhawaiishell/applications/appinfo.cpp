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

#include <QtCore/QFileInfo>
#include <QtCore/QStandardPaths>

#include "appinfo.h"
#include "xdgdesktopfile.h"

namespace Hawaii {

namespace Shell {

/*
 * AppInfoPrivate
 */

class AppInfoPrivate
{
public:
    AppInfoPrivate();
    ~AppInfoPrivate();

    XdgDesktopFile *entry;
};

AppInfoPrivate::AppInfoPrivate()
    : entry(new XdgDesktopFile())
{
}

AppInfoPrivate::~AppInfoPrivate()
{
    delete entry;
}

/*
 * AppInfo
 */

AppInfo::AppInfo(QObject *parent)
    : QObject(parent)
    , d_ptr(new AppInfoPrivate())
{
}

AppInfo::~AppInfo()
{
    delete d_ptr;
}

bool AppInfo::load(const QString &fileName)
{
    Q_D(AppInfo);
    return d->entry->load(fileName);
}

QString AppInfo::fileName() const
{
    Q_D(const AppInfo);
    return d->entry->fileName();
}

QString AppInfo::name() const
{
    Q_D(const AppInfo);
    return d->entry->name();
}

QString AppInfo::genericName() const
{
    Q_D(const AppInfo);
    return d->entry->localizedValue(QStringLiteral("GenericName")).toString();
}

QString AppInfo::iconName() const
{
    Q_D(const AppInfo);
    return d->entry->iconName();
}

QString AppInfo::comment() const
{
    Q_D(const AppInfo);
    return d->entry->comment();
}

QStringList AppInfo::categories() const
{
    Q_D(const AppInfo);
    return d->entry->value(QStringLiteral("Categories")).toString().split(';', QString::SkipEmptyParts);
}

bool AppInfo::isShow(const QString &name) const
{
    Q_D(const AppInfo);
    return d->entry->isShow(name);
}

bool AppInfo::isExecutable() const
{
    Q_D(const AppInfo);

    QString tryExec = d->entry->value(QStringLiteral("TryExec")).toString();
    QString execCommand = d->entry->value(QStringLiteral("Exec")).toString();

    // If TryExec is empty fallback to the first Exec argument
    if (tryExec.isEmpty()) {
        QStringList args = execCommand.split(QLatin1Char(' '), QString::SkipEmptyParts);
        if (args.size() == 0)
            return false;
        tryExec = args.at(0);
    }

    // Check if a full path was provided
    QFileInfo info(tryExec);
    if (info.exists())
        return QFileInfo(tryExec).isExecutable();

    // Find the executable from the system paths
    info.setFile(QStandardPaths::findExecutable(tryExec));
    if (info.exists())
        return info.isExecutable();

    return false;
}

bool AppInfo::isHidden() const
{
    Q_D(const AppInfo);

    // Application exists but should be hidden in the menu
    if (d->entry->value(QStringLiteral("NoDisplay")).toBool())
        return true;

    // User deleted this application at his level
    if (d->entry->value(QStringLiteral("Hidden")).toBool())
        return true;

    return false;
}

bool AppInfo::launch(const QStringList &arguments)
{
    Q_D(AppInfo);
    return d->entry->startDetached(arguments);
}

} // namespace Shell

} // namespace Hawaii

#include "moc_appinfo.cpp"
