// SPDX-FileCopyrightText: 2018 Pier Luigi Fiorini <pierluigi.fiorini@gmail.com>
//
// SPDX-License-Identifier: GPL-3.0-or-later

#ifndef CHROMEITEM_H
#define CHROMEITEM_H

#include <QQuickItem>
#include <QWaylandCompositor>
#include <QWaylandQuickItem>

class ChromeItem : public QQuickItem
{
    Q_OBJECT
    Q_PROPERTY(QWaylandCompositor *compositor READ compositor NOTIFY compositorChanged)
    Q_PROPERTY(QWaylandQuickItem *shellSurfaceItem READ shellSurfaceItem WRITE setShellSurfaceItem NOTIFY shellSurfaceItemChanged)
    Q_PROPERTY(bool primary READ isPrimary NOTIFY primaryChanged)
public:
    enum ShellSurfaceType {
        WaylandShellSurface = 0,
        XWaylandShellSurface
    };
    Q_ENUM(ShellSurfaceType)

    ChromeItem(QQuickItem *parent = nullptr);

    QWaylandCompositor *compositor() const;

    QWaylandQuickItem *shellSurfaceItem() const;
    void setShellSurfaceItem(QWaylandQuickItem *item);

    bool isPrimary() const;

    Q_INVOKABLE QPointF randomPosition(const QPointF &mousePos) const;

Q_SIGNALS:
    void compositorChanged();
    void shellSurfaceItemChanged();
    void primaryChanged();

public Q_SLOTS:
    void raise();
    void lower();
    void takeFocus(QWaylandSeat *device = nullptr);

private:
    QWaylandCompositor *m_compositor = nullptr;
    QWaylandQuickItem *m_shellSurfaceItem = nullptr;
    bool m_primary = false;

private Q_SLOTS:
    void updatePrimary();
};

#endif // CHROMEITEM_H
