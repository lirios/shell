/****************************************************************************
 * This file is part of Hawaii Shell.
 *
 * Copyright (C) 2014 Pier Luigi Fiorini <pierluigi.fiorini@gmail.com>
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

#include <QtCore/QDebug>
#include <QtCore/QFile>
#include <QtCore/QJsonArray>
#include <QtCore/QJsonDocument>
#include <QtCore/QJsonObject>
#include <QtCore/QSharedData>
#include <QtCore/QVariant>

#include "pluginmetadata.h"

namespace Hawaii {

namespace Shell {

/*
 * PluginMetadataPrivate
 */

class PluginMetadataPrivate : public QSharedData
{
public:
    PluginMetadataPrivate();

    /************************************************
     * LC_MESSAGES value	Possible keys in order of matching
     * lang_COUNTRY@MODIFIER	lang_COUNTRY@MODIFIER, lang_COUNTRY, lang@MODIFIER, lang,
     *                        default value
     * lang_COUNTRY	        lang_COUNTRY, lang, default value
     * lang@MODIFIER	        lang@MODIFIER, lang, default value
     * lang	                lang, default value
     *
     * This function is derivate from libqtxdg code.
     * Copyright (C) 2010-2011 Razor team
     * Author: Alexander Sokoloff <sokoloff.a@gmail.com>
     ************************************************/
    QStringList currentLanguage() const
    {
        QStringList list;
        QString lang = QString::fromUtf8(getenv("LC_MESSAGES"));

        if (lang.isEmpty())
            lang = QString::fromUtf8(getenv("LC_ALL"));
        if (lang.isEmpty())
            lang = QString::fromUtf8(getenv("LANG"));

        QString modifier = lang.section('@', 1);
        if (!modifier.isEmpty())
            lang.truncate(lang.length() - modifier.length() - 1);

        QString encoding = lang.section('.', 1);
        if (!encoding.isEmpty())
            lang.truncate(lang.length() - encoding.length() - 1);


        QString country = lang.section('_', 1);
        if (!country.isEmpty())
            lang.truncate(lang.length() - country.length() - 1);

        if (!modifier.isEmpty() && !country.isEmpty())
            list.append(QString("%1_%2@%3").arg(lang, country, modifier));

        if (!country.isEmpty())
            list.append(QString("%1_%2").arg(lang, country));

        if (!modifier.isEmpty())
            list.append(QString("%1@%2").arg(lang, modifier));

        list.append(lang);

        return list;
    }

    QVariant localizedValue(const QString &key) const
    {
        if (!valid)
            return QVariant();

        QStringList langs = currentLanguage();
        langs.append(QStringList("en"));

        QJsonObject description = pluginInfo.value(QStringLiteral("Description")).toObject();
        for (QString lang: description.keys()) {
            if (langs.contains(lang)) {
                QJsonObject record = description.value(lang).toObject();
                QString value = record.value(key).toString();
                if (!value.isEmpty())
                    return value;
            }
        }

        return QVariant();
    }

    bool valid;
    QFile file;
    QJsonObject json;
    QJsonObject pluginInfo;
};

PluginMetadataPrivate::PluginMetadataPrivate()
    : valid(false)
{
}

/*
 * PluginMetadata
 */

PluginMetadata::PluginMetadata(const QString &fileName)
    : d_ptr(new PluginMetadataPrivate())
{
    load(fileName);
}

PluginMetadata::PluginMetadata()
    : d_ptr(new PluginMetadataPrivate())
{
}

bool PluginMetadata::load(const QString &fileName)
{
    Q_D(PluginMetadata);

    // Open the file
    d->file.setFileName(fileName);
    if (!d->file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        qWarning("Couldn't open \"%s\": %s",
                 qPrintable(fileName),
                 qPrintable(d->file.errorString()));
        d->valid = false;
        return false;
    }

    QByteArray data = d->file.readAll();
    d->file.close();

    // Load the JSON document
    QJsonParseError error;
    QJsonDocument doc = QJsonDocument::fromJson(data, &error);
    if (error.error != QJsonParseError::NoError) {
        qWarning("Error parsing \"%s\" at offset %d: %s",
                 qPrintable(fileName),
                 error.offset,
                 qPrintable(error.errorString()));
        d->valid = false;
        return false;
    }

    // Retrieve the JSON object
    d->json = doc.object();
    d->pluginInfo = d->json.value(QStringLiteral("PluginInfo")).toObject();

    d->valid = !d->pluginInfo.value(QStringLiteral("Implements")).toString().isEmpty() &&
            !d->pluginInfo.value(QStringLiteral("InternalName")).toString().isEmpty();
    if (!d->valid)
        qWarning() << "Metadata" << fileName << "is not valid!";
    return d->valid;
}

bool PluginMetadata::isValid() const
{
    Q_D(const PluginMetadata);
    return d->valid;
}

QString PluginMetadata::name() const
{
    Q_D(const PluginMetadata);
    if (!d->valid)
        return QString();
    return d->localizedValue(QStringLiteral("Name")).toString();
}

QString PluginMetadata::comment() const
{
    Q_D(const PluginMetadata);
    if (!d->valid)
        return QString();
    return d->localizedValue(QStringLiteral("Comment")).toString();
}

QStringList PluginMetadata::keywords() const
{
    Q_D(const PluginMetadata);
    if (!d->valid)
        return QStringList();
    return d->localizedValue(QStringLiteral("Keywords")).toStringList();
}

QString PluginMetadata::iconName() const
{
    Q_D(const PluginMetadata);
    if (!d->valid)
        return QString();
    return d->pluginInfo.value(QStringLiteral("Icon")).toString();
}

QString PluginMetadata::implements() const
{
    Q_D(const PluginMetadata);
    return d->pluginInfo.value(QStringLiteral("Implements")).toString();
}

QString PluginMetadata::internalName() const
{
    Q_D(const PluginMetadata);
    if (!d->valid)
        return QString();
    return d->pluginInfo.value(QStringLiteral("InternalName")).toString();
}

QString PluginMetadata::version() const
{
    Q_D(const PluginMetadata);
    if (!d->valid)
        return QString();
    return d->pluginInfo.value(QStringLiteral("Version")).toString();
}

QString PluginMetadata::license() const
{
    Q_D(const PluginMetadata);
    if (!d->valid)
        return QString();
    return d->pluginInfo.value(QStringLiteral("License")).toString();
}

QStringList PluginMetadata::authors() const
{
    Q_D(const PluginMetadata);
    if (!d->valid)
        return QStringList();

    QStringList results;
    QVariantList variantList = d->pluginInfo.value(QStringLiteral("Authors")).toArray().toVariantList();
    for (QVariant variant: variantList)
        results.append(variant.toString());
    return results;
}

QString PluginMetadata::contactEmail() const
{
    Q_D(const PluginMetadata);
    if (!d->valid)
        return QString();
    return d->pluginInfo.value(QStringLiteral("Contact")).toObject().value(QStringLiteral("Email")).toString();
}

QString PluginMetadata::contactWebsite() const
{
    Q_D(const PluginMetadata);
    if (!d->valid)
        return QString();
    return d->pluginInfo.value(QStringLiteral("Contact")).toObject().value(QStringLiteral("Website")).toString();
}

QString PluginMetadata::mainScript() const
{
    Q_D(const PluginMetadata);
    if (!d->valid)
        return QString();
    return d->pluginInfo.value(QStringLiteral("MainScript")).toString();
}

QVariant PluginMetadata::value(const QString &key) const
{
    Q_D(const PluginMetadata);
    if (!d->valid)
        return QVariant();
    return d->json.value(key).toVariant();
}

} // namespace Shell

} // namespace Hawaii
