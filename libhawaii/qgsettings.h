/****************************************************************************
 * This file is part of Hawaii.
 *
 * Copyright (C) 2015 Pier Luigi Fiorini
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

#ifndef HAWAII_QGSETTINGS_H
#define HAWAII_QGSETTINGS_H

#include <QtCore/QObject>

#include <hawaii/hawaii_export.h>

namespace Hawaii {

class QGSettingsPrivate;

class HAWAII_EXPORT QGSettings : public QObject
{
    Q_OBJECT
    Q_PROPERTY(bool valid READ isValid CONSTANT)
public:
    QGSettings(const QString &schemaId, const QString &path = QString(),
               QObject *parent = 0);
    ~QGSettings();

    bool isValid() const;

    QVariant value(const QString &key) const;
    void setValue(const QString &key, const QVariant &value);

    bool trySetValue(const QString &key, const QVariant &value);

    QVariant defaultValue(const QString &key) const;

    QVariantList choices(const QString &key) const;

    void resetValue(const QString &key);

    QStringList keys() const;

    static QStringList schemas();
    static bool isSchemaInstalled(const QString &schemaId);

Q_SIGNALS:
    void settingChanged(const QString &key);

private:
    Q_DECLARE_PRIVATE(QGSettings)
    QGSettingsPrivate *const d_ptr;
};

} // namespace Hawaii

#endif // HAWAII_QGSETTINGS_H
