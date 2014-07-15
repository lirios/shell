/****************************************************************************
 * This file is part of Hawaii Shell.
 *
 * Copyright (C) 2013-2014 Pier Luigi Fiorini <pierluigi.fiorini@gmail.com>
 *
 * Author(s):
 *    Pier Luigi Fiorini
 *
 * $BEGIN_LICENSE:GPL2+$
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 *
 * $END_LICENSE$
 ***************************************************************************/

#ifndef POPUPGRABBER_H
#define POPUPGRABBER_H

#include <QtCompositor/QWaylandInputDevice>
#include <QtCompositor/private/qwlpointer_p.h>

class ShellPopupSurface;

class PopupGrabber : public QtWayland::PointerGrabber
{
public:
    PopupGrabber(QtWayland::InputDevice *inputDevice);

    QtWayland::InputDevice *inputDevice() const;

    uint32_t grabSerial() const;

    struct ::wl_client *client() const;
    void setClient(struct ::wl_client *client);

    ShellPopupSurface *popupSurface() const;
    void setPopupSurface(ShellPopupSurface *popupSurface);

    void end();
    void endGrab();

    void focus() Q_DECL_OVERRIDE;
    void motion(uint32_t time) Q_DECL_OVERRIDE;
    void button(uint32_t time, Qt::MouseButton button, uint32_t state) Q_DECL_OVERRIDE;

private:
    QtWayland::InputDevice *m_inputDevice;
    struct ::wl_client *m_client;
    ShellPopupSurface * m_popupSurface;
    bool m_inside;
    uint32_t m_creationTime;
};

#endif // POPUPGRABBER_H
