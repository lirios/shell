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

#include <HawaiiShell/Export>

namespace Hawaii {

namespace Shell {

class Element;
class ElementItemPrivate;

class HAWAIISHELL_EXPORT ElementItem : public QQuickItem
{
    Q_OBJECT
    Q_PROPERTY(bool busy READ isBusy NOTIFY busyChanged)
public:
    explicit ElementItem(Element *element, QQuickItem *parent = 0);
    ~ElementItem();

    Element *element() const;

    bool isBusy() const;

    void setContextProperty(const QString &name, const QVariant &value);

    void initialize();

    static ElementItem *qmlAttachedProperties(QObject *object);

Q_SIGNALS:
    void busyChanged();

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
