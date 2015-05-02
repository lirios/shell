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

#include "qgsettings.h"
#include "qmlgsettings.h"
#include "qmlgsettingsschema.h"

QmlGSettings::QmlGSettings(QObject *parent)
    : QQmlPropertyMap(this, parent)
    , m_schema(new QmlGSettingsSchema(this))
    , m_settings(Q_NULLPTR)
{
}

QmlGSettingsSchema *QmlGSettings::schema() const
{
    return m_schema;
}

void QmlGSettings::classBegin()
{
}

void QmlGSettings::componentComplete()
{
    bool installed = Hawaii::QGSettings::isSchemaInstalled(m_schema->id());
    if (installed) {
        // Load settings
        m_settings = new Hawaii::QGSettings(m_schema->id(), m_schema->path(), this);
        if (!m_settings->isValid())
            return;
        connect(m_settings, &Hawaii::QGSettings::settingChanged, this, [this](const QString &key) {
            // Get the key value
            QVariant newValue = m_settings->value(key);

            // And change the property if it's different
            bool changed = false;
            if (value(key) != newValue) {
                insert(key, newValue);
                changed = true;
            }
            if (changed)
                Q_EMIT settingsChanged();
        });

        // Insert all the keys
        Q_FOREACH (const QString &key, m_settings->keys())
            insert(key, m_settings->value(key));
    }

    // Set schema validity
    m_schema->m_valid = installed;
    Q_EMIT m_schema->validChanged();
    Q_EMIT schemaChanged();
}

QVariant QmlGSettings::updateValue(const QString &key, const QVariant &input)
{
    // Sanity check
    if (!m_settings)
        return QVariant();

    // Update settings when a value has changed here
    if (m_settings->trySetValue(key, input))
        return input;

    // Unable to set value, return old value
    qCWarning(SETTINGS) << "Unable to set value for" << key;
    return m_settings->value(key);
}

#include "moc_qmlgsettings.cpp"
