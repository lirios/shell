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

#ifndef SHELLSETTINGS_H
#define SHELLSETTINGS_H

#include <QtCore/QObject>

#include <HawaiiShell/Export>

class ShellSettingsPrivate;

class HAWAIISHELL_EXPORT ShellSettings : public QObject
{
    Q_OBJECT
    Q_PROPERTY(QString style READ style WRITE setStyle NOTIFY styleChanged)
    Q_PROPERTY(QString handler READ handler WRITE setHandler NOTIFY handlerChanged)
    Q_DECLARE_PRIVATE(ShellSettings)
public:
    explicit ShellSettings(QObject *parent = 0);
    ~ShellSettings();

    QString style() const;
    void setStyle(const QString &style);

    QString handler() const;
    void setHandler(const QString &handler);

Q_SIGNALS:
    void styleChanged();
    void handlerChanged();

private:
    ShellSettingsPrivate *const d_ptr;
};

#endif // SHELLSETTINGS_H
