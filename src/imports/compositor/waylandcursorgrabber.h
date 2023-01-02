// SPDX-FileCopyrightText: 2021 Pier Luigi Fiorini <pierluigi.fiorini@gmail.com>
//
// SPDX-License-Identifier: GPL-3.0-or-later

#ifndef WAYLANDCURSORGRABBER_H
#define WAYLANDCURSORGRABBER_H

#include <LiriAuroraCompositor/WaylandQuickItem>
#include <LiriAuroraCompositor/WaylandSeat>
#include <LiriAuroraCompositor/WaylandSurfaceGrabber>

class WaylandCursorGrabber : public Aurora::Compositor::WaylandQuickItem
{
    Q_OBJECT
    Q_PROPERTY(Aurora::Compositor::WaylandSeat *seat READ seat WRITE setSeat NOTIFY seatChanged)
    Q_PROPERTY(int hotspotX READ hotspotX NOTIFY hotspotXChanged)
    Q_PROPERTY(int hotspotY READ hotspotY NOTIFY hotspotYChanged)
    Q_PROPERTY(bool grab READ grab WRITE setGrab NOTIFY grabChanged)
public:
    WaylandCursorGrabber(QQuickItem *parent = nullptr);

    Aurora::Compositor::WaylandSeat *seat() const;
    void setSeat(Aurora::Compositor::WaylandSeat *seat);

    int hotspotX() const;
    int hotspotY() const;

    bool grab() const;
    void setGrab(bool value);

Q_SIGNALS:
    void seatChanged(Aurora::Compositor::WaylandSeat *seat);
    void hotspotXChanged(int hotspotX);
    void hotspotYChanged(int hotspotY);
    void grabChanged(bool grab);

private:
    Aurora::Compositor::WaylandSurfaceGrabber *m_grabber = nullptr;
    Aurora::Compositor::WaylandSurfaceGrabber *m_dragIconGrabber = nullptr;
    bool m_grabbing = false;
    Aurora::Compositor::WaylandSeat *m_seat = nullptr;
    int m_hotspotX = -1;
    int m_hotspotY = -1;
    bool m_grab = false;

private Q_SLOTS:
    void handleCursorSurfaceRequest(Aurora::Compositor::WaylandSurface *cursorSurface, int hotspotX, int hotspotY);
    void handleDragIconChanged();
    void handleRedraw();
    void handleGrab(const QImage &image);
};

#endif // WAYLANDCURSORGRABBER_H
