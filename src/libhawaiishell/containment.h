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

#include <HawaiiShell/Mantle>
#include <HawaiiShell/HawaiiShell>

namespace Hawaii {

namespace Shell {

class ContainmentPrivate;
class Element;

class HAWAIISHELL_EXPORT Containment : public QObject
{
    Q_OBJECT
    Q_PROPERTY(Hawaii::Shell::Types::ContainmentType type READ type NOTIFY typeChanged)
    Q_PROPERTY(Hawaii::Shell::Types::FormFactor formFactor READ formFactor WRITE setFormFactor NOTIFY formFactorChanged)
    Q_PROPERTY(Hawaii::Shell::Types::Location location READ location WRITE setLocation NOTIFY locationChanged)
    Q_PROPERTY(bool immutable READ isImmutable WRITE setImmutable NOTIFY immutableChanged)
    Q_PROPERTY(bool configuring READ isConfiguring WRITE setConfiguring NOTIFY configuringChanged)
public:
    explicit Containment(Mantle *mantle, QObject *parent = 0);
    ~Containment();

    /*!
     * \return the mantle for this containment.
     */
    Mantle *mantle() const;

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
     * \return whether the containment is immutable or not.
     */
    bool isImmutable() const;

    /*!
     * Makes the containment immutable or not.
     * Immutable containments cannot be configured by the user.
     * \param value whether the containment is immutable or not
     */
    void setImmutable(bool value);

    /*!
     * \return whether this containment is in configuration mode.
     */
    bool isConfiguring() const;

    /*!
     * Enable or disable the configuration mode.
     * \param value whether configuration mode is enabled or not.
     */
    void setConfiguring(bool value);

    /*!
     * \return the package loaded for this containment.
     */
    Package package() const;

    /*!
     * Loads a QML file from the package.
     * \param package package to load the QML file from.
     */
    void setPackage(const Package &package);

    /*!
     * Adds the element called \a name.
     * \param name internal name of the element to add
     *             (something along the line of org.hawaii.elements.anelement).
     */
    void addElement(const QString &name);

    /*!
     * Removes the \a element.
     * \param element the element to remove.
     */
    void removeElement(Element *element);

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
     * Emitted when the containment becomes immutable or it's no longer
     * immutable.
     * \param newValue whether the containment is immutable
     */
    void immutableChanged(bool newValue);

    /*!
     * Emitted when the configuration mode is enabled or disabled.
     * \param newValue whether the containment is now in configuration mode or not.
     */
    void configuringChanged(bool newValue);

    /*!
     * Emitted when the package for this containment is changed.
     * \param package the new package.
     */
    void packageChanged(const Package &package);

    /*!
     * Emitted when an element is added.
     * \param element the elmement.
     */
    void elementAdded(Hawaii::Shell::Element *element);

    /*!
     * Emitted when an element is removed.
     * \param element the elmement.
     */
    void elementRemoved(Hawaii::Shell::Element *element);

private:
    Q_DECLARE_PRIVATE(Containment)
    ContainmentPrivate *const d_ptr;
};

} // namespace Shell

} // namespace Hawaii

#endif // CONTAINMENT_H
