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

#include "pluginmetadata.h"
#include "xdgdesktopfile.h"

namespace Hawaii {

namespace Shell {

/*
 * PluginMetadataPrivate
 */

class PluginMetadataPrivate
{
public:
    PluginMetadataPrivate();
    ~PluginMetadataPrivate();

    XdgDesktopFile *entry;
    PluginMetadata::Type type;
};

PluginMetadataPrivate::PluginMetadataPrivate()
    : entry(nullptr)
    , type(PluginMetadata::InvalidType)
{
}

PluginMetadataPrivate::~PluginMetadataPrivate()
{
    delete entry;
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

PluginMetadata::~PluginMetadata()
{
    //delete d_ptr;
}

bool PluginMetadata::load(const QString &fileName)
{
    Q_D(PluginMetadata);

    if (!d->entry)
        d->entry = new XdgDesktopFile();
    if (d->entry->load(fileName)) {
        if (d->entry->value(QStringLiteral("Type")).toString() != QStringLiteral("Service")) {
            // Unload metadata if it's not the right type
            delete d->entry;
            d->entry = nullptr;
        } else {
            // Determine the plugin type
            QString type = d->entry->value(QStringLiteral("X-Hawaii-ServiceType")).toString();
            if (type.startsWith(QStringLiteral("Hawaii/Shell"))) {
                if (type.endsWith(QStringLiteral("/Element")))
                    d->type = PluginMetadata::ElementType;
                else if (type.endsWith(QStringLiteral("/Containment")))
                    d->type = PluginMetadata::ContainmentType;
                else if (type.endsWith(QStringLiteral("/Shell")))
                    d->type = PluginMetadata::ShellType;
                else if (type.endsWith(QStringLiteral("/LookAndFeel")))
                    d->type = PluginMetadata::LookAndFeelType;
                else
                    d->type = PluginMetadata::InvalidType;
            } else {
                d->type = PluginMetadata::InvalidType;
            }
        }

        return true;
    }

    return false;
}

bool PluginMetadata::isValid() const
{
    Q_D(const PluginMetadata);
    return d->entry && d->entry->isValid();
}

PluginMetadata::Type PluginMetadata::type() const
{
    Q_D(const PluginMetadata);
    return d->type;
}

QString PluginMetadata::name() const
{
    Q_D(const PluginMetadata);
    if (!d->entry)
        return QString();
    return d->entry->name();
}

QString PluginMetadata::comment() const
{
    Q_D(const PluginMetadata);
    if (!d->entry)
        return QString();
    return d->entry->comment();
}

QString PluginMetadata::iconName() const
{
    Q_D(const PluginMetadata);
    if (!d->entry)
        return QString();
    return d->entry->iconName();
}

QString PluginMetadata::internalName() const
{
    Q_D(const PluginMetadata);
    if (!d->entry)
        return QString();
    return d->entry->value(QStringLiteral("X-Hawaii-PluginInfo-Name")).toString();
}

QString PluginMetadata::version() const
{
    Q_D(const PluginMetadata);
    if (!d->entry)
        return QString();
    return d->entry->value(QStringLiteral("X-Hawaii-PluginInfo-Version")).toString();
}

QString PluginMetadata::license() const
{
    Q_D(const PluginMetadata);
    if (!d->entry)
        return QString();
    return d->entry->value(QStringLiteral("X-Hawaii-PluginInfo-License")).toString();
}

QString PluginMetadata::authorName() const
{
    Q_D(const PluginMetadata);
    if (!d->entry)
        return QString();
    return d->entry->value(QStringLiteral("X-Hawaii-PluginInfo-AuthorName")).toString();
}

QString PluginMetadata::authorEmail() const
{
    Q_D(const PluginMetadata);
    if (!d->entry)
        return QString();
    return d->entry->value(QStringLiteral("X-Hawaii-PluginInfo-AuthorEmail")).toString();
}

QString PluginMetadata::webSite() const
{
    Q_D(const PluginMetadata);
    if (!d->entry)
        return QString();
    return d->entry->value(QStringLiteral("X-Hawaii-PluginInfo-WebSite")).toString();
}

QVariant PluginMetadata::property(const QString &key) const
{
    Q_D(const PluginMetadata);
    if (!d->entry)
        return QVariant();
    return d->entry->localizedValue(key);
}

} // namespace Shell

} // namespace Hawaii
