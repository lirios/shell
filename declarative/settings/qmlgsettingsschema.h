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

#ifndef QMLGSETTINGSSCHEMA_H
#define QMLGSETTINGSSCHEMA_H

#include <QtCore/QLoggingCategory>
#include <QtCore/QObject>

Q_DECLARE_LOGGING_CATEGORY(SETTINGS)

class QmlGSettings;

class QmlGSettingsSchema : public QObject
{
    Q_OBJECT
    Q_PROPERTY(bool valid READ isValid NOTIFY validChanged)
    Q_PROPERTY(QString id READ id WRITE setId)
    Q_PROPERTY(QString path READ path WRITE setPath)
    Q_PROPERTY(QStringList keys READ keys NOTIFY keysChanged)
public:
    QmlGSettingsSchema(QObject *parent = 0);
    ~QmlGSettingsSchema();

    bool isValid() const;

    QString id() const;
    void setId(const QString &id);

    QString path() const;
    void setPath(const QString &path);

    QStringList keys() const;

Q_SIGNALS:
    void validChanged();
    void keysChanged();

private:
    bool m_valid;
    QString m_schemaId;
    QString m_path;
    QStringList m_keys;

    friend class QmlGSettings;
};

#endif // QMLGSETTINGSSCHEMA_H
