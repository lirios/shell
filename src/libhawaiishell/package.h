/****************************************************************************
 * This file is part of Hawaii Shell.
 *
 * Copyright (C) 2014 Pier Luigi Fiorini <pierluigi.fiorini@gmail.com>
 * Copyright (C) 2007-2011 Aaron Seigo <aseigo@kde.org>
 *
 * Author(s):
 *    Pier Luigi Fiorini
 *
 * $BEGIN_LICENSE:LGPL2+$
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License as published by
 * the Free Software Foundation, either version 2 of the License, or
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

#ifndef PACKAGE_H
#define PACKAGE_H

#include <QtCore/QExplicitlySharedDataPointer>
#include <QtCore/QStringList>

#include <HawaiiShell/Export>
#include <HawaiiShell/PluginMetadata>

namespace Hawaii {

namespace Shell {

class PackagePrivate;
class PackageStructure;

/**
 * @short object representing an installed Hawaii Shell package
 *
 * Package defines what is in a package and provides easy access to the contents.
 *
 * To define a package, one might write the following code:
 *
 @code
    Package package;

    package.addDirectoryDefinition("images", "pics/", i18n("Images"));
    QStringList mimeTypes;
    mimeTypes << "image/svg" << "image/png" << "image/jpeg";
    package.setMimeTypes("images", mimeTypes);

    package.addDirectoryDefinition("scripts", "code/", i18n("Executable Scripts"));
    mimeTypes.clear();
    mimeTypes << "text/\*";
    package.setMimeTypes("scripts", mimeTypes);

    package.addFileDefinition("mainscript", "code/main.js", i18n("Main Script File"));
    package.setRequired("mainscript", true);
 @endcode
 * One may also choose to create a subclass of PackageStructure and include the setup
 * in the constructor.
 *
 * Either way, Package creates a self-documenting contract between the packager and
 * the application without exposing package internals such as actual on-disk structure
 * of the package or requiring that all contents be explicitly known ahead of time.
 *
 * Subclassing PackageStructure does have provide a number of potential const benefits:
 *    * the package can be notified of path changes via the virtual pathChanged() method
 *    * the subclass may implement mechanisms to install and remove packages using the
 *      virtual install and uninstall methods
 *    * subclasses can be compiled as plugins for easy re-use
 **/
class HAWAIISHELL_EXPORT Package
{
public:
    /**
     * Default constructor
     *
     * @arg structure if a NULL pointer is passed in, this will creates an empty (invalid) Package;
     * otherwise the structure is allowed to set up the Package's initial layout
     */
    explicit Package(PackageStructure *structure = 0);

    /**
     * Copy constructor
     */
    Package(const Package &other);

    ~Package();

    /**
     * Assignment operator
     */
    Package &operator=(const Package &rhs);

    /**
     * @return true if all the required components exist
     **/
    bool isValid() const;

    /**
     * Sets the path to the root of this package
     * @param path an absolute path, or a relative path to the default package root
     */
    void setPath(const QString &path);

    /**
     * @return the path to the root of this particular package
     */
    const QString path() const;

    /**
     * Get the path to a given file based on the key and an optional filename.
     * Example: finding the main script in a scripting package:
     *      filePath("mainscript")
     *
     * Example: finding a specific image in the images directory:
     *      filePath("images", "myimage.png")
     *
     * @param key the key of the file type to look for,
     * @param filename optional name of the file to locate within the package
     * @return path to the file on disk. QString() if not found.
     **/
    QString filePath(const char *key, const QString &filename = QString()) const;

    /**
     * Get the list of files of a given type.
     *
     * @param fileType the type of file to look for, as defined in the
     *               package structure.
     * @return list of files by name, suitable for passing to filePath
     **/
    QStringList entryList(const char *key) const;

    /**
     * @return user visible name for the given entry
     **/
    QString name(const char *key) const;

    /**
     * @return true if the item at path exists and is required
     **/
    bool isRequired(const char *key) const;

    /**
     * @return the mimeTypes associated with the path, if any
     **/
    QStringList mimeTypes(const char *key) const;

    /**
     * @return the prefix paths inserted between the base path and content entries, in order of priority.
     *         When searching for a file, all paths will be tried in order.
     */
    QStringList contentsPrefixPaths() const;

    /**
     * @return preferred package root. This defaults to hawaii/elements/
     */
    QString defaultPackageRoot() const;

    /**
     * @return true if paths/symlinks outside the package itself should be followed.
     * By default this is set to false for security reasons.
     */
    bool allowExternalPaths() const;

    /**
      * @return the package metadata object.
      */
    PluginMetadata metadata() const;

    /**
     * @return a SHA1 hash digest of the contents of the package in hexadecimal form
     */
    QString contentsHash() const;

    /**
     * Adds a directory to the structure of the package. It is added as
     * a not-required element with no associated mimeTypes.
     *
     * If an entry with the given key already exists, the path is added to
     * it as a search alternative.
     *
     * @param key  used as an internal label for this directory
     * @param path the path within the package for this directory
     * @param name the user visible (translated) name for the directory
     **/
    void addDirectoryDefinition(const char *key, const QString &path, const QString &name);

    /**
     * Adds a file to the structure of the package. It is added as
     * a not-required element with no associated mimeTypes.
     *
     * If an entry with the given key already exists, the path is added to
     * it as a search alternative.
     *
     * @param key  used as an internal label for this file
     * @param path the path within the package for this file
     * @param name the user visible (translated) name for the file
     **/
    void addFileDefinition(const char *key, const QString &path, const QString &name);

    /**
     * Removes a definition from the structure of the package.
     * @param key the internal label of the file or directory to remove
     */
    void removeDefinition(const char *key);

    /**
     * Sets whether or not a given part of the structure is required or not.
     * The path must already have been added using addDirectoryDefinition
     * or addFileDefinition.
     *
     * @param key the entry within the package
     * @param required true if this entry is required, false if not
     */
    void setRequired(const char *key, bool required);

    /**
     * Defines the default mimeTypes for any definitions that do not have
     * associated mimeTypes. Handy for packages with only one or predominantly
     * one file type.
     *
     * @param mimeTypes a list of mimeTypes
     **/
    void setDefaultMimeTypes(QStringList mimeTypes);

    /**
     * Define mimeTypes for a given part of the structure
     * The path must already have been added using addDirectoryDefinition
     * or addFileDefinition.
     *
     * @param key the entry within the package
     * @param mimeTypes a list of mimeTypes
     **/
    void setMimeTypes(const char *key, QStringList mimeTypes);

    /**
     * Sets the prefixes that all the contents in this package should
     * appear under. This defaults to "contents/" and is added automatically
     * between the base path and the entries as defined by the package
     * structure. Multiple entries can be added.
     * In this case each file request will be searched in all prefixes in order,
     * and the first found will be returned.
     *
     * @param prefix paths the directory prefix to use
     */
    void setContentsPrefixPaths(const QStringList &prefixPaths);

    /**
     * Sets whether or not external paths/symlinks can be followed by a package
     * @param allow true if paths/symlinks outside of the package should be followed,
     *             false if they should be rejected.
     */
    void setAllowExternalPaths(bool allow);

    /**
     * Sets preferred package root.
     */
    void setDefaultPackageRoot(const QString &packageRoot);

    // Content structure description methods
    /**
     * @return all directories registered as part of this Package's structure
     */
    QList<const char *> directories() const;

    /**
     * @return all directories registered as part of this Package's required structure
     */
    QList<const char *> requiredDirectories() const;

    /**
     * @return all files registered as part of this Package's structure
     */
    QList<const char *> files() const;

    /**
     * @return all files registered as part of this Package's required structure
     */
    QList<const char *> requiredFiles() const;

private:
    QExplicitlySharedDataPointer<PackagePrivate> d;
};

} // namespace Shell

} // namespace Hawaii

Q_DECLARE_METATYPE(Hawaii::Shell::Package)

#endif // PACKAGE_H
