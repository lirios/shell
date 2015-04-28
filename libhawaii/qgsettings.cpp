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

#include "logging.h"
#include "qgvariantutils.h"
#include "qgsettings.h"
#include "utils.h"

extern "C" {
#include <gio/gio.h>
}

Q_LOGGING_CATEGORY(QGSETTINGS, "hawaii.qgsettings")

namespace Hawaii {

/*
 * QGSettingsPrivate
 */

class QGSettingsPrivate
{
public:
    QGSettingsPrivate(const QString &schemaId, const QString &path, QGSettings *q)
        : schemaId(schemaId)
        , path(path)
        , valid(false)
        , settings(Q_NULLPTR)
        , schema(Q_NULLPTR)
    {
        if (path.isEmpty())
            settings = g_settings_new(schemaId.toUtf8().constData());
        else
            settings = g_settings_new_with_path(schemaId.toUtf8().constData(),
                                                path.toUtf8().constData());
        if (settings) {
            g_object_get(settings, "settings-schema", &schema, Q_NULLPTR);
            g_signal_connect(settings, "changed",
                             G_CALLBACK(QGSettingsPrivate::settingChanged), q);
        }

        valid = settings && schema;
    }

    ~QGSettingsPrivate()
    {
        g_settings_sync();
        if (settings)
            g_object_unref(settings);
        if (schema)
            g_settings_schema_unref(schema);
    }

    static void settingChanged(GSettings *settings, const gchar *key, gpointer data)
    {
        Q_UNUSED(settings)

        QGSettings *self = static_cast<QGSettings *>(data);
        Q_EMIT self->settingChanged(Utils::toCamelCase(key));
    }

    QString schemaId;
    QString path;
    bool valid;
    GSettings *settings;
    GSettingsSchema *schema;
};

/*
 * QGSettings
 */

QGSettings::QGSettings(const QString &schemaId, const QString &path, QObject *parent)
    : QObject(parent)
    , d_ptr(new QGSettingsPrivate(schemaId, path, this))
{
}

QGSettings::~QGSettings()
{
    delete d_ptr;
}

bool QGSettings::isValid() const
{
    Q_D(const QGSettings);
    return d->valid;
}

QVariant QGSettings::value(const QString &key) const
{
    Q_D(const QGSettings);

    if (!d->valid)
        return QVariant();

    gchar *keyName = Utils::fromCamelCase(key);
    GVariant *gvalue = g_settings_get_value(d->settings, keyName);
    QVariant qvalue = Utils::convertValue(gvalue);
    g_variant_unref(gvalue);
    g_free(keyName);
    return qvalue;
}

void QGSettings::setValue(const QString &key, const QVariant &value)
{
    if (!trySetValue(key, value)) {
        qCWarning(QGSETTINGS) << "Unable to set value for key" << key;
        return;
    }
}

bool QGSettings::trySetValue(const QString &key, const QVariant &value)
{
    Q_D(QGSettings);

    if (!d->valid)
        return false;

    bool result = false;

    gchar *keyName = Utils::fromCamelCase(key);
    GVariant *oldValue = g_settings_get_value(d->settings, keyName);
    GVariant *newValue = Utils::convertVariant(value);
    if (newValue)
        result = g_settings_set_value(d->settings, keyName, newValue);

    g_free(keyName);
    g_variant_unref(oldValue);

    return result;
}

QVariant QGSettings::defaultValue(const QString &key) const
{
    Q_D(const QGSettings);

    if (!d->valid)
        return QVariant();

    gchar *keyName = Utils::fromCamelCase(key);
    GVariant *gvalue = g_settings_get_default_value(d->settings, keyName);
    QVariant qvalue = Utils::convertValue(gvalue);
    g_variant_unref(gvalue);
    g_free(keyName);
    return qvalue;
}

QVariantList QGSettings::choices(const QString &key) const
{
    Q_D(const QGSettings);

    QVariantList choices;

    if (!d->valid)
        return choices;

    gchar *keyName = Utils::fromCamelCase(key);
    GSettingsSchemaKey *schemaKey = g_settings_schema_get_key(d->schema, keyName);
    GVariant *range = g_settings_schema_key_get_range(schemaKey);
    g_settings_schema_key_unref(schemaKey);
    g_free(keyName);

    if (!range)
        return choices;

    const gchar *type;
    GVariant *value;
    g_variant_get(range, "(&sv)", &type, &value);

    if (g_str_equal(type, "enum")) {
        GVariantIter iter;
        GVariant *child;

        g_variant_iter_init(&iter, value);
        while ((child = g_variant_iter_next_value(&iter))) {
            choices.append(Utils::convertValue(child));
            g_variant_unref(child);
        }
    }

    g_variant_unref(value);
    g_variant_unref(range);

    return choices;
}

void QGSettings::resetValue(const QString &key)
{
    Q_D(QGSettings);

    if (!d->valid)
        return;

    gchar *keyName = Utils::fromCamelCase(key);
    g_settings_reset(d->settings, keyName);
    g_free(keyName);
}

QStringList QGSettings::keys() const
{
    Q_D(const QGSettings);

    QStringList result;

    if (!d->valid)
        return result;

    gchar **keys = g_settings_list_keys(d->settings);
    for (int i = 0; keys[i]; i++)
        result.append(Utils::toCamelCase(keys[i]));
    g_strfreev(keys);
    return result;
}

QStringList QGSettings::schemas()
{
    GSettingsSchemaSource *source = g_settings_schema_source_get_default();
    gchar **schemas;
    g_settings_schema_source_list_schemas(source, true, &schemas, Q_NULLPTR);
    QStringList result;
    for (int i = 0; schemas[i]; i++)
        result.append(schemas[i]);
    g_strfreev(schemas);
    return result;
}

bool QGSettings::isSchemaInstalled(const QString &schemaId)
{
    GSettingsSchemaSource *source = g_settings_schema_source_get_default();
    GSettingsSchema *schema = g_settings_schema_source_lookup(source, schemaId.toUtf8().constData(), true);
    if (schema) {
        g_settings_schema_unref(schema);
        return true;
    } else {
        return false;
    }
}

} // namespace Hawaii

#include "moc_qgsettings.cpp"
