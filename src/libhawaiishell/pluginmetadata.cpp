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

#include <QDebug>
#include <QtCore/QFile>
#include <QtCore/QJsonArray>
#include <QtCore/QJsonDocument>
#include <QtCore/QJsonObject>
#include <QtCore/QSharedData>

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

        QVariantMap translations = json.value(QStringLiteral("Translations")).toObject().toVariantMap();
        QStringList langs = currentLanguage();
        for (QString lang: translations.keys()) {
            if (langs.contains(lang)) {
                QMap<QString, QVariant> map = translations[lang].toMap();
                if (map.contains(key))
                    return translations[lang].toMap()[key];
                return json.value(key);
            }
        }

        return json.value(key);
    }

    bool valid;
    QFile file;
    QJsonObject json;
    PluginMetadata::Type type;
};

PluginMetadataPrivate::PluginMetadataPrivate()
    : valid(false)
    , type(PluginMetadata::InvalidType)
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

    // Determine the plugin type
    QString type = d->json.value(QStringLiteral("Type")).toString();
    if (type == QStringLiteral("background"))
        d->type = PluginMetadata::BackgroundType;
    else if (type == QStringLiteral("element"))
        d->type = PluginMetadata::ElementType;
    else if (type == QStringLiteral("containment"))
        d->type = PluginMetadata::ContainmentType;
    else if (type == QStringLiteral("shell"))
        d->type = PluginMetadata::ShellType;
    else if (type == QStringLiteral("lookandfeel"))
        d->type = PluginMetadata::LookAndFeelType;
    else if (type == QStringLiteral("preferences"))
        d->type = PluginMetadata::PreferencesType;
    else
        d->type = PluginMetadata::InvalidType;

    d->valid = d->type != PluginMetadata::InvalidType;
    return true;
}

bool PluginMetadata::isValid() const
{
    Q_D(const PluginMetadata);
    return d->valid;
}

PluginMetadata::Type PluginMetadata::type() const
{
    Q_D(const PluginMetadata);
    return d->type;
}

QString PluginMetadata::name() const
{
    Q_D(const PluginMetadata);
    if (!d->valid)
        return QString();
    return d->localizedValue(QStringLiteral("Title")).toString();
}

QString PluginMetadata::comment() const
{
    Q_D(const PluginMetadata);
    if (!d->valid)
        return QString();
    return d->localizedValue(QStringLiteral("Description")).toString();
}

QString PluginMetadata::iconName() const
{
    Q_D(const PluginMetadata);
    if (!d->valid)
        return QString();
    return d->json.value(QStringLiteral("IconName")).toString();
}

QString PluginMetadata::internalName() const
{
    Q_D(const PluginMetadata);
    if (!d->valid)
        return QString();
    return d->json.value(QStringLiteral("Name")).toString();
}

QString PluginMetadata::version() const
{
    Q_D(const PluginMetadata);
    if (!d->valid)
        return QString();
    return d->json.value(QStringLiteral("Version")).toString();
}

QString PluginMetadata::license() const
{
    Q_D(const PluginMetadata);
    if (!d->valid)
        return QString();
    return d->json.value(QStringLiteral("License")).toString();
}

QList<QStringList> PluginMetadata::authors() const
{
    Q_D(const PluginMetadata);

    QList<QStringList> list;
    if (!d->valid)
        return list;

    QJsonArray authors = d->json.value(QStringLiteral("Authors")).toArray();
    for (QVariant item: authors.toVariantList()) {
        QStringList items = QStringList()
                << item.toMap().value(QStringLiteral("Name")).toString()
                << item.toMap().value(QStringLiteral("Email")).toString();
        list.append(items);
    }
    return list;
}

QString PluginMetadata::webSite() const
{
    Q_D(const PluginMetadata);
    if (!d->valid)
        return QString();
    return d->json.value(QStringLiteral("WebSite")).toString();
}

QString PluginMetadata::mainScript() const
{
    Q_D(const PluginMetadata);
    if (!d->valid)
        return QString();
    return d->json.value(QStringLiteral("MainScript")).toString();
}

QVariant PluginMetadata::property(const QString &key) const
{
    Q_D(const PluginMetadata);
    if (!d->valid)
        return QVariant();
    return d->localizedValue(key);
}

} // namespace Shell

} // namespace Hawaii
