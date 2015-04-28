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

#include "qmlgsettingsschema.h"

Q_LOGGING_CATEGORY(SETTINGS, "hawaii.qml.settings")

QmlGSettingsSchema::QmlGSettingsSchema(QObject *parent)
    : QObject(parent)
    , m_valid(false)
{
}

QmlGSettingsSchema::~QmlGSettingsSchema()
{
}

bool QmlGSettingsSchema::isValid() const
{
    return m_valid;
}

QString QmlGSettingsSchema::id() const
{
    return m_schemaId;
}

void QmlGSettingsSchema::setId(const QString &id)
{
    if (!m_schemaId.isEmpty()) {
        qCWarning(SETTINGS) << "Schema identifier can only be set once, at creation time";
        return;
    }

    m_schemaId = id;
}

QString QmlGSettingsSchema::path() const
{
    return m_path;
}

void QmlGSettingsSchema::setPath(const QString &path)
{
    if (!m_path.isEmpty()) {
        qCWarning(SETTINGS) << "Schema path can only be set once, at creation time";
        return;
    }

    m_path = path;
}

#include "moc_qmlgsettingsschema.cpp"
