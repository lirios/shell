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

#ifndef ELEMENTITEM_H
#define ELEMENTITEM_H

#include <QtQuick/QQuickItem>

#include <HawaiiShell/HawaiiShell>

namespace Hawaii {

namespace Shell {

class Element;
class ElementItemPrivate;

class HAWAIISHELL_EXPORT ElementItem : public QQuickItem
{
    Q_OBJECT
    Q_PROPERTY(uint id READ id CONSTANT)
    Q_PROPERTY(QQuickItem *rootObject READ rootObject NOTIFY rootObjectChanged)
    Q_PROPERTY(Hawaii::Shell::Types::FormFactor formFactor READ formFactor NOTIFY formFactorChanged)
    Q_PROPERTY(Hawaii::Shell::Types::Location location READ location NOTIFY locationChanged)
    Q_PROPERTY(QString title READ title WRITE setTitle NOTIFY titleChanged)
    Q_PROPERTY(QString label READ label WRITE setLabel NOTIFY labelChanged)
    Q_PROPERTY(QString iconName READ iconName WRITE setIconName NOTIFY iconNameChanged)
    Q_PROPERTY(bool busy READ isBusy NOTIFY busyChanged)
public:
    explicit ElementItem(Element *element, QQuickItem *parent = 0);
    ~ElementItem();

    Element *element() const;

    /*!
     * Returns the element identifier.
     * This identifier is number unique to the session but it may
     * change between restarts.
     * \return the element identifier.
     */
    uint id() const;

    /*!
     * \return the element root object.
     */
    QQuickItem *rootObject() const;

    /*!
     * \return the form factor of the containment.
     */
    Hawaii::Shell::Types::FormFactor formFactor() const;

    /*!
     * \return the location of the containment.
     */
    Hawaii::Shell::Types::Location location() const;

    bool isBusy() const;

    /*!
     * Descriptive title of this element.
     * The localized name from element's metadata is returned by default.
     * \return the title of this element.
     */
    QString title() const;

    /*!
     * Sets a descriptive title of this element.
     * \param title the title of this element.
     */
    void setTitle(const QString &title);

    /*!
     * Label that describes this elment or shows a summary of its
     * contents.  This is property is handy for elements that will
     * be represented as indicators; think of a clock element shown
     * by a status menu, the label may return current date and time
     * and when clicked the element reveals a calendar.
     * The localized description from element's metadata is returned by
     * default.
     * \return the label describing this element.
     */
    QString label() const;

    /*!
     * Sets a label that describes this element or shows a summary
     * of its contents.
     * The localized description from element's metadata is returned by
     * default.
     * \param label the label describing this element.
     */
    void setLabel(const QString &label);

    /*!
     * Name of the icon that represents this element.
     * Icon names must respect the XDG Icon Naming Specification.
     * The icon name from element's metadata is returned by default.
     * \return the name of the icon representing this element.
     */
    QString iconName() const;

    /*!
     * Sets the name of the icon representing this element.
     * Icon names must respect the XDG Icon Naming Specification.
     * \param iconName the name of the icon representing this element.
     */
    void setIconName(const QString &iconName);

    /*!
     * Sets the context property \a name to \a value.
     * \param name name of the context property.
     * \param value value of the context property.
     */
    void setContextProperty(const QString &name, const QVariant &value);

    void initialize();

    static ElementItem *qmlAttachedProperties(QObject *object);

Q_SIGNALS:
    /*!
     * Emitted when the root object is changed (e.g. the package has changed)
     */
    void rootObjectChanged(QQuickItem *rootObject);

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

    void busyChanged();

    /*!
     * Emitted when the element title is changed.
     * \param title the new element title.
     */
    void titleChanged(const QString &title);

    /*!
     * Emitted when the element label is changed.
     * \param label the new element labe.
     */
    void labelChanged(const QString &label);

    /*!
     * Emitted when the icon name representing this element is changed.
     * \param iconName the new element icon name.
     */
    void iconNameChanged(const QString &iconName);

protected:
    virtual void itemChange(ItemChange change, const ItemChangeData &value);

private:
    Q_DECLARE_PRIVATE(ElementItem)
    ElementItemPrivate *const d_ptr;

    Q_PRIVATE_SLOT(d_func(), void _q_minimumWidthChanged())
    Q_PRIVATE_SLOT(d_func(), void _q_minimumHeightChanged())
    Q_PRIVATE_SLOT(d_func(), void _q_preferredWidthChanged())
    Q_PRIVATE_SLOT(d_func(), void _q_preferredHeightChanged())
    Q_PRIVATE_SLOT(d_func(), void _q_maximumWidthChanged())
    Q_PRIVATE_SLOT(d_func(), void _q_maximumHeightChanged())
    Q_PRIVATE_SLOT(d_func(), void _q_fillWidthChanged())
    Q_PRIVATE_SLOT(d_func(), void _q_fillHeightChanged())
};

} // namespace Shell

} // namespace Hawaii

QML_DECLARE_TYPEINFO(Hawaii::Shell::ElementItem, QML_HAS_ATTACHED_PROPERTIES)

#endif // ELEMENTITEM_H
