/****************************************************************************
 * This file is part of Liri.
 *
 * Copyright (C) 2018 Pier Luigi Fiorini <pierluigi.fiorini@gmail.com>
 *
 * $BEGIN_LICENSE:LGPLv3+$
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
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

#ifndef LIRI_OUTPUTCONFIGURATIONADAPTOR_H
#define LIRI_OUTPUTCONFIGURATIONADAPTOR_H

#include <QDBusObjectPath>
#include <QObject>

class OutputConfiguration;

class OutputConfigurationAdaptor : public QObject
{
    Q_OBJECT
    Q_CLASSINFO("D-Bus Interface", "io.liri.Shell.OutputConfiguration1")
public:
    explicit OutputConfigurationAdaptor(OutputConfiguration *parent);
    ~OutputConfigurationAdaptor();

    QDBusObjectPath path() const;

    Q_SCRIPTABLE void SetEnabled(const QDBusObjectPath &handle, bool enabled);
    Q_SCRIPTABLE void SetMode(const QDBusObjectPath &handle, int index);
    Q_SCRIPTABLE void SetTransform(const QDBusObjectPath &handle, const QString &transform);
    Q_SCRIPTABLE void SetPosition(const QDBusObjectPath &handle, const QPoint &position);
    Q_SCRIPTABLE void SetScaleFactor(const QDBusObjectPath &handle, int scaleFactor);

    Q_SCRIPTABLE void Apply();

Q_SIGNALS:
    void Applied();
    void Failed();

private:
    OutputConfiguration *m_parent = nullptr;
    int m_id = 0;
};

#endif // LIRI_OUTPUTCONFIGURATIONADAPTOR_H
