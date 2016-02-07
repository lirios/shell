/****************************************************************************
 * This file is part of Hawaii.
 *
 * Copyright (C) 2015-2016 Pier Luigi Fiorini
 *
 * Author(s):
 *    Pier Luigi Fiorini <pierluigi.fiorini@gmail.com>
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

#ifndef TRIANGLE_H
#define TRIANGLE_H

#include <QtQuick/QQuickItem>
#include <QtQuick/QSGFlatColorMaterial>
#include <QtQuick/QSGGeometry>

class Triangle : public QQuickItem
{
    Q_OBJECT
    Q_PROPERTY(QColor color READ color WRITE setColor NOTIFY colorChanged)
    Q_PROPERTY(QPointF point1 READ point1 WRITE setPoint1 NOTIFY point1Changed)
    Q_PROPERTY(QPointF point2 READ point2 WRITE setPoint2 NOTIFY point2Changed)
    Q_PROPERTY(QPointF point3 READ point3 WRITE setPoint3 NOTIFY point3Changed)
public:
    Triangle(QQuickItem *parent = 0);

    QColor color() const;
    void setColor(const QColor &c);

    QPointF point1() const;
    void setPoint1(const QPointF &pt);

    QPointF point2() const;
    void setPoint2(const QPointF &pt);

    QPointF point3() const;
    void setPoint3(const QPointF &pt);

Q_SIGNALS:
    void colorChanged();
    void point1Changed();
    void point2Changed();
    void point3Changed();

protected:
    QSGNode *updatePaintNode(QSGNode *node, UpdatePaintNodeData *data);

private:
    QSGGeometry m_geometry;
    QSGFlatColorMaterial m_material;
    QPointF m_p1, m_p2, m_p3;

    void calculateVertices();
};

#endif // TRIANGLE_H
