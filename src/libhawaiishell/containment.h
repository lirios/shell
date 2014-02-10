/****************************************************************************
 * This file is part of Hawaii Shell.
 *
 * Copyright (C) 2012-2014 Pier Luigi Fiorini <pierluigi.fiorini@gmail.com>
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

#ifndef CONTAINMENT_H
#define CONTAINMENT_H

#include <QtCore/QObject>

#include <HawaiiShell/Corona>
#include <HawaiiShell/HawaiiShell>

namespace Hawaii {

namespace Shell {

class ContainmentPrivate;

class HAWAIISHELL_EXPORT Containment : public QObject
{
    Q_OBJECT
    Q_PROPERTY(Hawaii::Shell::Types::ContainmentType type READ type NOTIFY typeChanged)
    Q_PROPERTY(Hawaii::Shell::Types::FormFactor formFactor READ formFactor WRITE setFormFactor NOTIFY formFactorChanged)
    Q_PROPERTY(Hawaii::Shell::Types::Location location READ location WRITE setLocation NOTIFY locationChanged)
public:
    explicit Containment(Corona *corona, QObject *parent = 0);
    ~Containment();

    /*!
     * \return the corona for this containment.
     */
    Corona *corona() const;

    /*!
     * \return the containment type.
     */
    Hawaii::Shell::Types::ContainmentType type() const;

    /*!
     * \return the form factor of this containment.
     */
    Hawaii::Shell::Types::FormFactor formFactor() const;

    /*!
     * Sets the form factor of this containment.
     * \param formFactor the form factor of the containment
     */
    void setFormFactor(Types::FormFactor formFactor);

    /*!
     * \return the location of this containment.
     */
    Hawaii::Shell::Types::Location location() const;

    /*!
     * Sets the location of this containment.
     * \param location the location of the containment
     */
    void setLocation(Types::Location location);

    /*!
     * Sets the context property \a name to \a value.
     * \param name name of the context property.
     * \param value value of the context property.
     */
    void setContextProperty(const QString &name, const QVariant &value);

    /*!
     * \return the package loaded for this containment.
     */
    Package package() const;

    /*!
     * Loads a QML file from the package.
     * \param package package to load the QML file from.
     */
    void setPackage(const Package &package);

Q_SIGNALS:
    /*!
     * Emitted when the containment type is changed.
     * Initially containments are of \a UnknownContainment type,
     * as soon as the package is loaded the types changes to
     * what is read from the metadata entry.
     * \sa setPackage()
     * \param type the new containment type.
     */
    void typeChanged(Hawaii::Shell::Types::ContainmentType type);

    /*!
     * Emitted when the form factor is changed.
     * \param formFactor the new form factorof the containment
     */
    void formFactorChanged(Hawaii::Shell::Types::FormFactor formFactor);

    /*!
     * Emitted when the location is changed.
     * \param location the new location of the containment
     */
    void locationChanged(Hawaii::Shell::Types::Location location);

    /*!
     * Emitted when the package for this containment is changed.
     * \param package the new package.
     */
    void packageChanged(const Package &package);

private:
    Q_DECLARE_PRIVATE(Containment)
    ContainmentPrivate *const d_ptr;
};

} // namespace Shell

} // namespace Hawaii

#endif // CONTAINMENT_H
