/****************************************************************************
 * This file is part of Hawaii.
 *
 * Copyright (C) 2015 Pier Luigi Fiorini
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

#include <QtQuick/QSGGeometryNode>

#include "triangle.h"

Triangle::Triangle(QQuickItem *parent)
    : QQuickItem(parent)
    , m_geometry(QSGGeometry::defaultAttributes_Point2D(), 3)
{
    setFlag(ItemHasContents);
    calculateVertices();
}

QColor Triangle::color() const
{
    return m_material.color();
}

void Triangle::setColor(const QColor &c)
{
    if (m_material.color() == c)
        return;

    m_material.setColor(c);
    Q_EMIT colorChanged();
}

QPointF Triangle::point1() const
{
    return m_p1;
}

void Triangle::setPoint1(const QPointF &pt)
{
    if (m_p1 == pt)
        return;

    m_p1 = pt;
    Q_EMIT point1Changed();
}

QPointF Triangle::point2() const
{
    return m_p2;
}

void Triangle::setPoint2(const QPointF &pt)
{
    if (m_p2 == pt)
        return;

    m_p2 = pt;
    Q_EMIT point2Changed();
}

QPointF Triangle::point3() const
{
    return m_p3;
}

void Triangle::setPoint3(const QPointF &pt)
{
    if (m_p3 == pt)
        return;

    m_p3 = pt;
    Q_EMIT point3Changed();
}

QSGNode *Triangle::updatePaintNode(QSGNode *node, UpdatePaintNodeData *data)
{
    if (!node)
        node = new QSGNode();

    QSGGeometryNode *geometryNode = new QSGGeometryNode();

    if (m_p1.isNull() || m_p2.isNull() || m_p3.isNull())
        calculateVertices();

    QSGGeometry::Point2D *v = m_geometry.vertexDataAsPoint2D();
    v[0].x = m_p1.x();
    v[0].y = m_p1.y();
    v[1].x = m_p2.x();
    v[1].y = m_p2.y();
    v[2].x = m_p3.x();
    v[2].y = m_p3.y();
    geometryNode->setGeometry(&m_geometry);
    geometryNode->setMaterial(&m_material);

    node->appendChildNode(geometryNode);
    return node;
}

void Triangle::calculateVertices()
{
    const QRectF rect = boundingRect();
    m_p1 = rect.bottomLeft();
    m_p2 = QPointF(rect.left() + rect.width()/2, rect.top());
    m_p3 = rect.bottomRight();
}

#include "moc_triangle.cpp"
