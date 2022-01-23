// SPDX-FileCopyrightText: 2021 Pier Luigi Fiorini <pierluigi.fiorini@gmail.com>
//
// SPDX-License-Identifier: GPL-3.0-or-later

#include "inputregion.h"
#include "shellhelperlogging_p.h"

/*
 * Rect
 */

Rect::Rect(QObject *parent)
    : QObject(parent)
{
}

qreal Rect::x() const
{
    return m_rect.x();
}

void Rect::setX(qreal x)
{
    if (m_rect.x() == x)
        return;

    m_rect.setX(x);
    emit xChanged(x);
}

qreal Rect::y() const
{
    return m_rect.y();
}

void Rect::setY(qreal y)
{
    if (m_rect.y() == y)
        return;

    m_rect.setY(y);
    emit yChanged(y);
}

qreal Rect::width() const
{
    return m_rect.width();
}

void Rect::setWidth(qreal width)
{
    if (m_rect.width() == width)
        return;

    m_rect.setWidth(width);
    emit widthChanged(width);
}

qreal Rect::height() const
{
    return m_rect.height();
}

void Rect::setHeight(qreal height)
{
    if (m_rect.height() == height)
        return;

    m_rect.setHeight(height);
    emit heightChanged(height);
}

QRectF Rect::rect() const
{
    return m_rect;
}

/*
 * InputRegion
 */

InputRegion::InputRegion(QObject *parent)
    : QObject(parent)
{
}

QWindow *InputRegion::window() const
{
    return m_window;
}

void InputRegion::setWindow(QWindow *window)
{
    if (m_window == window)
        return;

    if (m_initialized) {
        qCWarning(lcShellHelper, "Unable to change WindowMask::window after initialization");
        return;
    }

    m_window = window;
    Q_EMIT windowChanged(m_window);
}

QQmlListProperty<Rect> InputRegion::rects()
{
    return {
        this, this,
        &InputRegion::appendRect,
        &InputRegion::rectsCount,
        &InputRegion::rectAt,
        &InputRegion::clearRects,
        &InputRegion::replaceRect,
        &InputRegion::removeLastRect
    };
}

void InputRegion::componentComplete()
{
    if (m_initialized)
        return;

    // Find the window from the parent, if not specified
    if (!m_window) {
        for (auto *p = parent(); p != nullptr; p = p->parent()) {
            if (auto *w = qobject_cast<QWindow *>(p)) {
                setWindow(w);
                break;
            }
        }
    }

    // Initialize
    m_initialized = true;

    // Set initial mask based on properties set when the component is complete
    setInputRegion();
}

void InputRegion::setInputRegion()
{
    if (m_window) {
        QRegion region;
        for (auto *r : qAsConst(m_rects))
            region += r->rect().toRect();
        qCDebug(lcShellHelper) << "Set input region to:" << region;
        m_window->setMask(region);
    }
}

void InputRegion::appendRect(QQmlListProperty<Rect> *list, Rect *r)
{
    auto *inputRegion = reinterpret_cast<InputRegion *>(list->data);
    inputRegion->m_rects.append(r);
    inputRegion->setInputRegion();
}

int InputRegion::rectsCount(QQmlListProperty<Rect> *list)
{
    return reinterpret_cast<InputRegion *>(list->data)->m_rects.count();
}

Rect *InputRegion::rectAt(QQmlListProperty<Rect> *list, int index)
{
    return reinterpret_cast<InputRegion *>(list->data)->m_rects.at(index);
}

void InputRegion::clearRects(QQmlListProperty<Rect> *list)
{
    auto *inputRegion = reinterpret_cast<InputRegion *>(list->data);
    inputRegion->m_rects.clear();
    inputRegion->setInputRegion();
}

void InputRegion::replaceRect(QQmlListProperty<Rect> *list, int index, Rect *r)
{
    auto *inputRegion = reinterpret_cast<InputRegion *>(list->data);
    inputRegion->m_rects[index] = r;
    inputRegion->setInputRegion();
}

void InputRegion::removeLastRect(QQmlListProperty<Rect> *list)
{
    auto *inputRegion = reinterpret_cast<InputRegion *>(list->data);
    inputRegion->m_rects.removeLast();
    inputRegion->setInputRegion();
}
