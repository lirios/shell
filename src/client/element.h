/****************************************************************************
 * This file is part of Hawaii Shell.
 *
 * Copyright (C) 2012-2013 Pier Luigi Fiorini <pierluigi.fiorini@gmail.com>
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

#ifndef ELEMENT_H
#define ELEMENT_H

#include <QtQuick/QQuickItem>

class ElementFactory;
class HawaiiShell;

class Element : public QQuickItem
{
    Q_OBJECT
    Q_PROPERTY(Location location READ location NOTIFY locationChanged)
    Q_ENUMS(Location)
public:
    enum Location {
        LeftSide = 0,
        TopSide,
        RightSide,
        BottomSide,
        Floating
    };

    explicit Element(Element *parent = 0);

    Location location() const;

Q_SIGNALS:
    void locationChanged();

private:
    friend class ElementFactory;

    HawaiiShell *m_shell;
    Location m_location;
};

#endif // ELEMENT_H
