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

#ifndef CONTAINMENTITEM_H
#define CONTAINMENTITEM_H

#include <QtQuick/QQuickItem>

#include <HawaiiShell/HawaiiShell>

namespace Hawaii {

namespace Shell {

class Containment;
class ContainmentItemPrivate;

class HAWAIISHELL_EXPORT ContainmentItem : public QQuickItem
{
    Q_OBJECT
    Q_PROPERTY(Hawaii::Shell::Types::ContainmentType type READ type)
    Q_PROPERTY(Hawaii::Shell::Types::FormFactor formFactor READ formFactor WRITE setFormFactor NOTIFY formFactorChanged)
    Q_PROPERTY(Hawaii::Shell::Types::Location location READ location WRITE setLocation NOTIFY locationChanged)
    Q_PROPERTY(bool immutable READ isImmutable WRITE setImmutable NOTIFY immutableChanged)
    Q_PROPERTY(bool configuring READ isConfiguring WRITE setConfiguring NOTIFY configuringChanged)
public:
    /*!
     * \brief Constructor.
     * Creates the object.
     * \param containment the containment.
     * \param parent parent item.
     */
    explicit ContainmentItem(Containment *containment, QQuickItem *parent = 0);

    /*!
     * \brief Destructor.
     * Destroys the object.
     */
    ~ContainmentItem();

    /*!
     * \return the containment represented by this item.
     */
    Containment *containment() const;

    /*!
     * \return the containment type.
     */
    Hawaii::Shell::Types::ContainmentType type() const;

    /*!
     * \return the form factor of the containment.
     */
    Hawaii::Shell::Types::FormFactor formFactor() const;

    /*!
     * Sets the form factor of this containment.
     * \param formFactor the form factor of the containment
     */
    void setFormFactor(Types::FormFactor formFactor);

    /*!
     * \return the location of the containment.
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
     * Initializes the containment item.
     */
    void initialize();

    static ContainmentItem *qmlAttachedProperties(QObject *object);

Q_SIGNALS:
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

private:
    Q_DECLARE_PRIVATE(ContainmentItem)
    ContainmentItemPrivate *const d_ptr;

    Q_PRIVATE_SLOT(d_func(), void _q_packageChanged())
};

} // namespace Shell

} // namespace Hawaii

QML_DECLARE_TYPEINFO(Hawaii::Shell::ContainmentItem, QML_HAS_ATTACHED_PROPERTIES)

#endif // CONTAINMENTITEM_H
