/*
 *   Copyright 2011 Marco Martin <mart@kde.org>
 *   Copyright 2015 Luca Beltrame <lbeltrame@kde.org>
 *
 *   This program is free software; you can redistribute it and/or modify
 *   it under the terms of the GNU Library General Public License as
 *   published by the Free Software Foundation; either version 2, or
 *   (at your option) any later version.
 *
 *   This program is distributed in the hope that it will be useful,
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *   GNU General Public License for more details
 *
 *   You should have received a copy of the GNU Library General Public
 *   License along with this program; if not, write to the
 *   Free Software Foundation, Inc.,
 *   51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.
 */

#include "qimageitem.h"

#include <QPainter>


QImageItem::QImageItem(QQuickItem *parent)
    : QQuickPaintedItem(parent),
      m_smooth(false),
      m_fillMode(QImageItem::Stretch)
{
    setFlag(ItemHasContents, true);
}


QImageItem::~QImageItem()
{
}

void QImageItem::setImage(const QImage &image)
{
    bool oldImageNull = m_image.isNull();
    m_image = image;
    updatePaintedRect();
    update();
    emit nativeWidthChanged();
    emit nativeHeightChanged();
    emit imageChanged();
    if (oldImageNull != m_image.isNull()) {
        emit nullChanged();
    }
}

QImage QImageItem::image() const
{
    return m_image;
}

void QImageItem::resetImage()
{
    setImage(QImage());
}

void QImageItem::setSmooth(const bool smooth)
{
    if (smooth == m_smooth) {
        return;
    }
    m_smooth = smooth;
    update();
}

bool QImageItem::smooth() const
{
    return m_smooth;
}

int QImageItem::nativeWidth() const
{
    return m_image.size().width();
}

int QImageItem::nativeHeight() const
{
    return m_image.size().height();
}

QImageItem::FillMode QImageItem::fillMode() const
{
    return m_fillMode;
}

void QImageItem::setFillMode(QImageItem::FillMode mode)
{
    if (mode == m_fillMode) {
        return;
    }

    m_fillMode = mode;
    updatePaintedRect();
    update();
    emit fillModeChanged();
}

void QImageItem::paint(QPainter *painter)
{
    if (m_image.isNull()) {
        return;
    }
    painter->save();
    painter->setRenderHint(QPainter::Antialiasing, m_smooth);
    painter->setRenderHint(QPainter::SmoothPixmapTransform, m_smooth);

     if (m_fillMode == TileVertically) {
        painter->scale(width()/(qreal)m_image.width(), 1);
    }

    if (m_fillMode == TileHorizontally) {
        painter->scale(1, height()/(qreal)m_image.height());
    }

    if (m_fillMode >= Tile) {
        painter->drawTiledPixmap(m_paintedRect, QPixmap::fromImage(m_image));
    } else {
        painter->drawImage(m_paintedRect, m_image, m_image.rect());
    }

    painter->restore();
}

bool QImageItem::isNull() const
{
    return m_image.isNull();
}

int QImageItem::paintedWidth() const
{
    if (m_image.isNull()) {
        return 0;
    }

    return m_paintedRect.width();
}

int QImageItem::paintedHeight() const
{
    if (m_image.isNull()) {
        return 0;
    }

    return m_paintedRect.height();
}

void QImageItem::updatePaintedRect()
{

    if (m_image.isNull()) {
        return;
    }

    QRect sourceRect = m_image.rect();

    QRect destRect;

    switch (m_fillMode) {
    case PreserveAspectFit: {
        QSize scaled = m_image.size();

        scaled.scale(boundingRect().size().toSize(), Qt::KeepAspectRatio);
        destRect = QRect(QPoint(0, 0), scaled);
        destRect.moveCenter(boundingRect().center().toPoint());
        break;
    }
    case PreserveAspectCrop: {
        QSize scaled = m_image.size();

        scaled.scale(boundingRect().size().toSize(), Qt::KeepAspectRatioByExpanding);
        destRect = QRect(QPoint(0, 0), scaled);
        destRect.moveCenter(boundingRect().center().toPoint());
        break;
    }
    case TileVertically: {
        destRect = boundingRect().toRect();
        destRect.setWidth(destRect.width() / (width()/(qreal)m_image.width()));
        break;
    }
    case TileHorizontally: {
        destRect = boundingRect().toRect();
        destRect.setHeight(destRect.height() / (height()/(qreal)m_image.height()));
        break;
    }
    case Stretch:
    case Tile:
    default:
        destRect = boundingRect().toRect();
    }

    if (destRect != sourceRect) {
        m_paintedRect = destRect;
        emit paintedHeightChanged();
        emit paintedWidthChanged();
    }
}

void QImageItem::geometryChanged(const QRectF& newGeometry, const QRectF& oldGeometry)
{
    QQuickPaintedItem::geometryChanged(newGeometry, oldGeometry);
    updatePaintedRect();
}