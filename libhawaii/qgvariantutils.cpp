/****************************************************************************
 * This file is part of Hawaii.
 *
 * Copyright (C) 2013-2015 Pier Luigi Fiorini
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

#include <QtCore/QDataStream>
#include <QtCore/QDebug>
#include <QtCore/QUrl>
#include <QtCore/QStringList>

#include "qgvariantutils.h"

namespace Utils {

QVariant convertValue(GVariant *value)
{
    if (!value)
        return QVariant(QVariant::Invalid);

    switch (g_variant_classify(value)) {
    case G_VARIANT_CLASS_BOOLEAN:
        return QVariant((bool)g_variant_get_boolean(value));
    case G_VARIANT_CLASS_BYTE:
        return QVariant(QLatin1Char(g_variant_get_byte(value)));
    case G_VARIANT_CLASS_INT16:
        return QVariant((int)g_variant_get_int16(value));
    case G_VARIANT_CLASS_UINT16:
        return QVariant((uint)g_variant_get_uint16(value));
    case G_VARIANT_CLASS_INT32:
        return QVariant((int)g_variant_get_int32(value));
    case G_VARIANT_CLASS_UINT32:
        return QVariant((uint)g_variant_get_uint32(value));
    case G_VARIANT_CLASS_INT64:
        return QVariant((qlonglong)g_variant_get_int64(value));
    case G_VARIANT_CLASS_UINT64:
        return QVariant((qulonglong)g_variant_get_uint64(value));
    case G_VARIANT_CLASS_DOUBLE:
        return QVariant(g_variant_get_double(value));
    case G_VARIANT_CLASS_STRING:
        return QVariant(QString::fromUtf8(g_variant_get_string(value, NULL)));
    case G_VARIANT_CLASS_ARRAY:
        if (g_variant_is_of_type(value, G_VARIANT_TYPE_STRING_ARRAY)) {
            GVariantIter iter;
            QStringList list;
            const gchar *str;

            g_variant_iter_init(&iter, value);

            while (g_variant_iter_next(&iter, "&s", &str))
                list.append(QString::fromUtf8(str));

            return QVariant(list);
        } else if (g_variant_is_of_type(value, G_VARIANT_TYPE_BYTESTRING_ARRAY)) {
            GVariantIter iter;
            QVariantList list;
            const gchar *item;

            g_variant_iter_init(&iter, value);

            while (g_variant_iter_next(&iter, "&y", &item))
                list.append(QByteArray(item));

            return list;
#ifndef QT_NO_DATASTREAM
        } else if (g_variant_is_of_type(value, G_VARIANT_TYPE_BYTESTRING)) {
            QByteArray a(g_variant_get_bytestring(value));
            QDataStream s(&a, QIODevice::ReadOnly);
            s.setVersion(QDataStream::Qt_4_0);
            QVariant result;
            s >> result;
            return result;
#else
        Q_ASSERT(!"QConfiguration: Cannot load custom types without QDataStream support");
#endif
        }
    default:
        break;
    }

    return QVariant(QVariant::Invalid);
}

GVariant *convertVariant(const QVariant &variant)
{
    switch (variant.type()) {
    case QVariant::Bool:
        return g_variant_new_boolean(variant.toBool());
    case QVariant::Char:
        return g_variant_new_byte(variant.toChar().toLatin1());
    case QVariant::Int:
        return g_variant_new_int32(variant.toInt());
    case QVariant::UInt:
        return g_variant_new_uint32(variant.toUInt());
    case QVariant::LongLong:
        return g_variant_new_int64(variant.toLongLong());
    case QVariant::ULongLong:
        return g_variant_new_uint64(variant.toULongLong());
    case QVariant::Double:
        return g_variant_new_double(variant.toDouble());
    case QVariant::String:
        return g_variant_new_string(variant.toString().toUtf8().constData());
    case QVariant::StringList: {
        QStringList value = variant.toStringList();

        GVariantBuilder *builder = g_variant_builder_new(G_VARIANT_TYPE("as"));
        Q_FOREACH (const QString &item ,value)
            g_variant_builder_add(builder, "s", item.toUtf8().constData());

        GVariant *result = g_variant_new("as", builder);
        g_variant_builder_unref(builder);
        return result;
    }
    case QVariant::ByteArray:
        return g_variant_new_bytestring(variant.toByteArray().constData());
    case QVariant::Url:
        return g_variant_new_string(variant.toUrl().toString().toUtf8().constData());
    case QVariant::Color:
        return g_variant_new_string(variant.toString().toUtf8().constData());
    default: {
#ifndef QT_NO_DATASTREAM
        QByteArray a;
        QDataStream s(&a, QIODevice::WriteOnly);
        s.setVersion(QDataStream::Qt_4_0);
        s << variant;
        return g_variant_new_from_data(G_VARIANT_TYPE_BYTESTRING, a.constData(),
                                       a.size(), TRUE, NULL, NULL);
#else
        Q_ASSERT(!"QConfiguration: Cannot save custom types without QDataStream support");
#endif
    }
    }

    return 0;
}

}
