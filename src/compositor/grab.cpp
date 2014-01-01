/****************************************************************************
 * This file is part of Hawaii Shell.
 *
 * Copyright (C) 2013-2014 Pier Luigi Fiorini <pierluigi.fiorini@gmail.com>
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

#include <QtCompositor/QWaylandInputDevice>
#include <QtCompositor/private/qwlpointer_p.h>
#include <QtCompositor/private/qwlinputdevice_p.h>
#include <QtCompositor/private/qwlsurface_p.h>

#include "grab.h"
#include "compositor.h"

/*
 * ShellGrabber
 */

class ShellGrabber : public QtWayland::PointerGrabber
{
public:
    ShellGrabber(Grab *parent, QtWayland::InputDevice *inputDevice)
        : m_parent(parent)
        , m_inputDevice(inputDevice)
    {
    }

    QtWayland::InputDevice *inputDevice() const
    {
        return m_inputDevice;
    }

    uint32_t grabSerial() const
    {
        return m_inputDevice->pointerDevice()->grabSerial();
    }

    void endGrab()
    {
        m_pointer->endGrab();
    }

    void focus() Q_DECL_OVERRIDE
    {
        if (m_pointer->current()) {
            m_pointer->setFocus(m_pointer->current(), m_pointer->currentPosition());
            m_parent->send_focus(m_parent->resource()->handle,
                                 m_pointer->current()->resource()->handle,
                                 m_pointer->currentPosition().x(),
                                 m_pointer->currentPosition().y());
        } else {
            m_pointer->setFocus(0, QPointF(0, 0));
        }
    }

    void motion(uint32_t time) Q_DECL_OVERRIDE
    {
        m_pointer->motion(time);
        m_parent->send_motion(m_parent->resource()->handle, time,
                              m_pointer->currentPosition().x(),
                              m_pointer->currentPosition().y());
    }

    void button(uint32_t time, Qt::MouseButton button, uint32_t state) Q_DECL_OVERRIDE
    {
        if (m_pointer->focusResource())
            m_pointer->sendButton(time, button, state);

        // TODO: Convert button to Wayland buttons
        m_parent->send_button(m_parent->resource()->handle, time, button, state);
    }

private:
    Grab *m_parent;
    QtWayland::InputDevice *m_inputDevice;
};

/*
 * Grab
 */

Grab::Grab()
    : QtWaylandServer::wl_hawaii_grab()
    , m_grabber(new ShellGrabber(this, Compositor::instance()->defaultInputDevice()->handle()))
{
}

void Grab::hawaii_grab_end(Resource *resource)
{
    m_grabber->endGrab();
}
