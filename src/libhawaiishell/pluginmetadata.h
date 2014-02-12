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

#ifndef PLUGINMETADATA_H
#define PLUGINMETADATA_H

#include <QtCore/QString>
#include <QtCore/QVariant>

#include <HawaiiShell/Export>

namespace Hawaii {

namespace Shell {

class PluginMetadataPrivate;

/*!
 * \brief Information about a plugin.
 *
 * This class holds information about a plugin.
 */
class HAWAIISHELL_EXPORT PluginMetadata
{
public:
    enum Type {
        InvalidType = 0,
        ElementType,
        ContainmentType,
        ToolBoxType,
        ShellType,
        LookAndFeelType
    };

    /*!
     * Read plugin metadata from \a fileName.
     *
     * Plugin metadata are basically desktop entries and should be in
     * the following form:
     *
     * \code
     * [DesktopEntry]
     * Name=User Visible Name
     * Comment=Description of what the plugin does.
     * Type=Service
     *
     * X-Hawaii-ServiceType=...
     * X-Hawaii-PluginInfo-Name=internalname
     * X-Hawaii-PluginInfo-Version=Version number
     * X-Hawaii-PluginInfo-License=License name
     * X-Hawaii-PluginInfo-AuthorName=Author's Name
     * X-Hawaii-PluginInfo-AuthorEmail=Author's Email
     * X-Hawaii-PluginInfo-WebSite=http://plugin.org
     * \endcode
     *
     * The X-Hawaii-PluginInfo-ServiceType key can only contain
     * one of the following values:
     * \list
     *   \listitem Hawaii/Shell/Element
     *   \listitem Hawaii/Shell/Containment
     *   \listitem Hawaii/Shell/ToolBox
     *   \listitem Hawaii/Shell/Shell
     *   \listitem Hawaii/Shell/LookAndFeel
     * \endlist
     *
     * Use a descriptive name for X-Hawaii-PluginInfo-License such as GPL,
     * GPLv2, or GPLv2+ if the plugin is licensed under the terms of the
     * GNU General Public License v2 or (at your option) any later version.
     *
     * Don't use nick names for X-Hawaii-PluginInfo-Author, please use
     * your first and last name.
     *
     * \param fileName name with full path of the plugin metadata
     */
    explicit PluginMetadata(const QString &fileName);

    /*!
     * Creates an invalid plugin, with no metadata information.
     *
     * \sa isValid
     */
    PluginMetadata();

    /*!
     * Destructor.
     */
    ~PluginMetadata();

    /*!
     * Loads metadata from another file.
     * \param fileName plugin metadata file name with path
     */
    bool load(const QString &fileName);

    /*!
     * \return whether plugin metadata information is valid
     */
    bool isValid() const;

    /*!
     * \return plugin type.
     *
     * \sa Type
     */
    Type type() const;

    /*!
     * \return localized user visible name.
     */
    QString name() const;

    /*!
     * \return localized description.
     */
    QString comment() const;

    /*!
     * \return icon name.
     */
    QString iconName() const;

    /*!
     * \return internal name.
     */
    QString internalName() const;

    /*!
     * \return version.
     */
    QString version() const;

    /*!
     * \return license name.
     */
    QString license() const;

    /*!
     * \return author's name.
     */
    QString authorName() const;

    /*!
     * \return author's email.
     */
    QString authorEmail() const;

    /*!
     * \return web site.
     */
    QString webSite() const;

    /*!
     * Returns the value associated with \a key.
     * You can use this method if you want to read custom values.
     * \param key which key you want to know the value
     * \return the value associated with \a key.
     */
    QVariant property(const QString &key) const;

private:
    Q_DECLARE_PRIVATE(PluginMetadata)
    PluginMetadataPrivate *const d_ptr;
};

} // namespace Shell

} // namespace Hawaii

#endif // PLUGINMETADATA_H
