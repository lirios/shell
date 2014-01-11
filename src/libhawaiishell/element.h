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

#include <QtQuick/QQuickItem>

#include <HawaiiShell/Export>

namespace Hawaii {

namespace Shell {

class ElementPrivate;

class HAWAIISHELL_EXPORT Element : public QQuickItem
{
    Q_OBJECT
    Q_PROPERTY(qreal minimumWidth READ minimumWidth WRITE setMinimumWidth NOTIFY minimumWidthChanged)
    Q_PROPERTY(qreal maximumWidth READ maximumWidth WRITE setMaximumWidth NOTIFY maximumWidthChanged)
    Q_PROPERTY(qreal minimumHeight READ minimumHeight WRITE setMinimumHeight NOTIFY minimumHeightChanged)
    Q_PROPERTY(qreal maximumHeight READ maximumHeight WRITE setMaximumHeight NOTIFY maximumHeightChanged)
    Q_PROPERTY(bool fillWidth READ fillWidth WRITE setFillWidth NOTIFY fillWidthChanged)
    Q_PROPERTY(bool fillHeight READ fillHeight WRITE setFillHeight NOTIFY fillHeightChanged)
public:
    explicit Element(QQuickItem *parent = 0);
    ~Element();

    qreal minimumWidth() const;
    void setMinimumWidth(qreal value);

    qreal maximumWidth() const;
    void setMaximumWidth(qreal value);

    qreal minimumHeight() const;
    void setMinimumHeight(qreal value);

    qreal maximumHeight() const;
    void setMaximumHeight(qreal value);

    bool fillWidth() const;
    void setFillWidth(bool value);

    bool fillHeight() const;
    void setFillHeight(bool value);

Q_SIGNALS:
    void minimumWidthChanged();
    void maximumWidthChanged();
    void minimumHeightChanged();
    void maximumHeightChanged();
    void fillWidthChanged();
    void fillHeightChanged();

private:
    Q_DECLARE_PRIVATE(Element)
    ElementPrivate *const d_ptr;
};

} // namespace Shell

} // namespace Hawaii

#endif // ELEMENT_H
