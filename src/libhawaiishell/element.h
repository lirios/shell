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

#ifndef ELEMENT_H
#define ELEMENT_H

#include <QtCore/QObject>

#include <HawaiiShell/Export>

namespace Hawaii {

namespace Shell {

class Containment;
class ElementItem;
class ElementPrivate;
class Package;

class HAWAIISHELL_EXPORT Element : public QObject
{
    Q_OBJECT
    Q_PROPERTY(uint elementId READ elementId CONSTANT)
    Q_PROPERTY(QString errorString READ errorString NOTIFY errorStringChanged)
public:
    explicit Element(const QString &name,
                     Hawaii::Shell::Containment *containment,
                     QObject *parent = 0);
    ~Element();

    uint elementId() const;

    Package package() const;

    Containment *containment() const;

    QString errorString() const;

Q_SIGNALS:
    void errorStringChanged();

private:
    Q_DECLARE_PRIVATE(Element)
    ElementPrivate *const d_ptr;

private:
    friend class ElementItem;

    void setErrorString(const QString &error);
};

} // namespace Shell

} // namespace Hawaii

#endif // ELEMENT_H
