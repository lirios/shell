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

#include <QtCompositor/QWaylandSurface>
#include <QtCompositor/private/qwlinputdevice_p.h>
#include <QtCompositor/private/qwlsurface_p.h>

#include "popupgrabber.h"
#include "shellpopupsurface.h"
#include "shellsurface.h"

PopupGrabber::PopupGrabber(QtWayland::InputDevice *inputDevice)
    : QtWayland::PointerGrabber()
    , m_inputDevice(inputDevice)
    , m_client(nullptr)
    , m_popupSurface(nullptr)
    , m_inside(false)
{
    m_creationTime = m_inputDevice->pointerDevice()->grabTime();
}

QtWayland::InputDevice *PopupGrabber::inputDevice() const
{
    return m_inputDevice;
}

uint32_t PopupGrabber::grabSerial() const
{
    return m_inputDevice->pointerDevice()->grabSerial();
}

struct ::wl_client *PopupGrabber::client() const
{
    return m_client;
}

void PopupGrabber::setClient(struct ::wl_client *client)
{
    m_client = client;
}

ShellPopupSurface *PopupGrabber::popupSurface() const
{
    return m_popupSurface;
}

void PopupGrabber::setPopupSurface(ShellPopupSurface *popupSurface)
{
    if (popupSurface) {
        m_client = popupSurface->resource()->client();
        m_popupSurface = popupSurface;
        if (m_inputDevice)
            m_inputDevice->pointerDevice()->startGrab(this);
    } else {
        if (m_inputDevice)
            m_inputDevice->pointerDevice()->endGrab();
        m_client = nullptr;
        m_popupSurface = nullptr;
    }
}

void PopupGrabber::end()
{
    if (m_inputDevice)
        m_inputDevice->pointerDevice()->endGrab();

    if (m_popupSurface) {
        m_popupSurface->send_popup_done();
        delete m_popupSurface;
        m_popupSurface = nullptr;
    }

    m_client = nullptr;
    delete this;
}

void PopupGrabber::endGrab()
{
    if (m_inputDevice)
        m_inputDevice->pointerDevice()->endGrab();
}

void PopupGrabber::focus()
{
    if (m_popupSurface)
        m_inside = m_pointer->current()->waylandSurface() == m_popupSurface->surface();
    else
        m_inside = false;

    if (m_pointer->current() &&
            m_pointer->current()->resource()->client() == m_client)
        m_pointer->setFocus(m_pointer->current(), m_pointer->currentPosition());
    else
        m_pointer->setFocus(0, QPointF());
}

void PopupGrabber::motion(uint32_t time)
{
    m_pointer->motion(time);
}

void PopupGrabber::button(uint32_t time, Qt::MouseButton button, uint32_t state)
{
    if (m_pointer->focusResource())
        m_pointer->sendButton(time, button, state);

    if (state == QtWaylandServer::wl_pointer::button_state_released &&
            !m_inside && (time - m_creationTime > 500))
        end();
}
