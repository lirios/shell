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

#ifndef APPINFO_H
#define APPINFO_H

#include <QtCore/QObject>

#include <HawaiiShell/Export>

#include "xdgdesktopfile.h"

namespace Hawaii {

namespace Shell {

class HAWAIISHELL_EXPORT AppInfo : public QObject, public XdgDesktopFile
{
    Q_OBJECT
    Q_PROPERTY(QString name READ name NOTIFY nameChanged)
    Q_PROPERTY(QString genericName READ genericName NOTIFY genericNameChanged)
    Q_PROPERTY(QString iconName READ iconName NOTIFY iconNameChanged)
    Q_PROPERTY(QString comment READ comment NOTIFY commentChanged)
public:
    AppInfo(QObject *parent = 0);

    QString genericName() const;
    QStringList categories() const;

    bool isExecutable() const;
    bool isHidden() const;

Q_SIGNALS:
    void nameChanged(const QString &value);
    void genericNameChanged(const QString &value);
    void iconNameChanged(const QString &value);
    void commentChanged(const QString &value);

public Q_SLOTS:
    void launch();
};

} // namespace Shell

} // namespace Hawaii

#endif // APPINFO_H
