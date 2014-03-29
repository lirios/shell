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
        BackgroundType,
        ElementType,
        ContainmentType,
        ShellType,
        LookAndFeelType,
        PreferencesType
    };

    /*!
     * Read plugin metadata from \a fileName.
     *
     * Plugin metadata are JSON documents and should be in the following form:
     *
     * \code
     * {
     *     "Name": "internal name",
     *     "Type": "...",
     *     "Version": "version number",
     *     "License": "license name",
     *     "Title": "user visible name";
     *     "Description": "description of what the plugin does",
     *     "IconName": "icon name from Freedesktop.org Icon Naming Specification",
     *     "Authors": [
     *         {
     *             "Name": "author's name",
     *             "Email": "author's email address"
     *         },
     *     ],
     *     "WebSite": "http://plugin.org",
     *     "MainScript": "path/to/mainfile.ext"
     * }
     * \endcode
     *
     * The "Type" key can only contain one of the following values:
     * \list
     *   \listitem background
     *   \listitem element
     *   \listitem containment
     *   \listitem shell
     *   \listitem lookandfeel
     *   \listitem preferences
     * \endlist
     *
     * The "IconName" key is not mandatory, especially for non visual
     * items such as containments. It's generally used only for elements.
     *
     * Use a descriptive name for "License" such as GPL, GPLv2, or GPLv2+
     * if the plugin is licensed under the terms of the
     * GNU General Public License v2 or (at your option) any later version.
     *
     * Don't use nick names for "Name" in "Authors", please use your first
     * and last name.
     *
     * If "type" is "containment" you must add the "ContainmentType" key
     * with one of the following values:
     * \list
     *   \listitem desktop
     *   \listitem panel
     * \endlist
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
     * \return a list of authors, each item contains two
     *         values (author name and email).
     */
    QList<QStringList> authors() const;

    /*!
     * \return web site.
     */
    QString webSite() const;

    /*!
     * \return the main script.
     */
    QString mainScript() const;

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
