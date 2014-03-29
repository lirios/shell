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
    /*!
     * Read plugin metadata from \a fileName.
     *
     * Plugin metadata are JSON documents and should be in the following form:
     *
     * \code
     * {
     *   "PluginInfo": {
     *     "Authors": [
     *       "First Last <first.last@email>"
     *     ],
     *     "Contact": {
     *       "Email": "contact@email",
     *       "Website": "http://plugin.org",
     *     },
     *     "Description": {
     *       "en": {
     *         "Comment": "Configure desktop preferences",
     *         "Keywords": [
     *           "A keyword to find this plugin",
     *           "Another keyword",
     *           "Yet another keyword"
     *         ],
     *         "Name": "Desktop"
     *       }
     *     },
     *     "Icon": "icon-name-from-xdg-specs",
     *     "Implements": "PluginType",
     *     "InternalName": "org.some.plugins",
     *     "License": "GPLv2+",
     *     "MainScript": "path/to/mainfile.ext"
     *   },
     *   ...
     * }
     * \endcode
     *
     * Metadata has at least a "PluginInfo" key, but depending on the plugin type
     * it may have another dictionary at the same level.
     *
     * The "Authors" key is mandatory. Please don't use nick names and specify your
     * email address.
     *
     * The "Contact" is optional, omit it if you feel the list of authors is enough
     * for users to get in touch.
     *
     * The "Description" key is mandatory and it must have at least the "Name" and
     * "Comment" keys. The "Keywords" key is optional.
     *
     * The "Icon" key is not mandatory, especially for non visual plugins.
     *
     * Use a descriptive name for "License" such as GPL, GPLv2, or GPLv2+
     * if the plugin is licensed under the terms of the
     * GNU General Public License v2 or (at your option) any later version.
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
     * \return localized user visible name.
     */
    QString name() const;

    /*!
     * \return localized description.
     */
    QString comment() const;

    /*!
     * \return localized keywords.
     */
    QStringList keywords() const;

    /*!
     * \return icon name.
     */
    QString iconName() const;

    /*!
     * \return internal name.
     */
    QString internalName() const;

    /*!
     * \return the plugin type.
     */
    QString implements() const;

    /*!
     * \return version.
     */
    QString version() const;

    /*!
     * \return license name.
     */
    QString license() const;

    /*!
     * \return a list of authors.
     */
    QStringList authors() const;

    /*!
     * \return email.
     */
    QString contactEmail() const;

    /*!
     * \return web site.
     */
    QString contactWebsite() const;

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
    QVariant value(const QString &key) const;

private:
    Q_DECLARE_PRIVATE(PluginMetadata)
    PluginMetadataPrivate *const d_ptr;
};

} // namespace Shell

} // namespace Hawaii

#endif // PLUGINMETADATA_H
