// SPDX-FileCopyrightText: 2018 Pier Luigi Fiorini <pierluigi.fiorini@gmail.com>
//
// SPDX-License-Identifier: GPL-3.0-or-later

#ifndef CHROMEITEM_H
#define CHROMEITEM_H

#include <QQuickItem>
#include <LiriAuroraCompositor/WaylandCompositor>
#include <LiriAuroraCompositor/WaylandQuickItem>

class ChromeItem : public QQuickItem
{
    Q_OBJECT
    Q_PROPERTY(Aurora::Compositor::WaylandCompositor *compositor READ compositor NOTIFY compositorChanged)
    Q_PROPERTY(Aurora::Compositor::WaylandQuickItem *shellSurfaceItem READ shellSurfaceItem WRITE setShellSurfaceItem NOTIFY shellSurfaceItemChanged)
    Q_PROPERTY(bool primary READ isPrimary NOTIFY primaryChanged)
public:
    enum ShellSurfaceType {
        WaylandShellSurface = 0,
        XWaylandShellSurface
    };
    Q_ENUM(ShellSurfaceType)

    ChromeItem(QQuickItem *parent = nullptr);

    Aurora::Compositor::WaylandCompositor *compositor() const;

    Aurora::Compositor::WaylandQuickItem *shellSurfaceItem() const;
    void setShellSurfaceItem(Aurora::Compositor::WaylandQuickItem *item);

    bool isPrimary() const;

    Q_INVOKABLE QPointF randomPosition(const QPointF &mousePos) const;

Q_SIGNALS:
    void compositorChanged();
    void shellSurfaceItemChanged();
    void primaryChanged();

public Q_SLOTS:
    void raise();
    void lower();
    void takeFocus(Aurora::Compositor::WaylandSeat *device = nullptr);

private:
    Aurora::Compositor::WaylandCompositor *m_compositor = nullptr;
    Aurora::Compositor::WaylandQuickItem *m_shellSurfaceItem = nullptr;
    bool m_primary = false;

private Q_SLOTS:
    void updatePrimary();
};

#endif // CHROMEITEM_H
