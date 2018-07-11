/****************************************************************************
 * This file is part of Liri.
 *
 * Copyright (C) 2018 Pier Luigi Fiorini <pierluigi.fiorini@gmail.com>
 *
 * $BEGIN_LICENSE:LGPLv3+$
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
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

#include <QWaylandCompositor>
#include <QScreen>

#include <LiriPlatformHeaders/lirieglfsfunctions.h>

#include "screencast.h"
#include "screencastadaptor.h"
#ifdef SCREENCAST_ENABLED
#  include "screencaststream.h"
#endif

Q_LOGGING_CATEGORY(lcScreenCast, "liri.screencast", QtDebugMsg)

ScreenCast::ScreenCast(QObject *parent)
    : QObject(parent)
    , m_adaptor(new ScreenCastAdaptor(this))
#ifdef SCREENCAST_ENABLED
    , m_stream(new ScreenCastStream())
#endif
{
#ifdef SCREENCAST_ENABLED
    connect(m_stream, &ScreenCastStream::streamReady, this, [this] (quint32 nodeId) {
        qCInfo(lcScreenCast) << "PipeWire stream is ready:" << nodeId;
        Q_EMIT streamReady(nodeId);
    });
    connect(m_stream, &ScreenCastStream::startStreaming, this, [this] {
        if (!m_streamingEnabled) {
            m_streamingEnabled = true;
            qCInfo(lcScreenCast) << "PipeWire streaming has started";
            Q_EMIT startStreaming();
        }
    });
    connect(m_stream, &ScreenCastStream::stopStreaming, this, [this] {
        if (m_streamingEnabled) {
            m_streamingEnabled = false;
            m_stream->removeStream();
            qCInfo(lcScreenCast) << "PipeWire streaming has stopped";
            Q_EMIT stopStreaming();
        }
    });

    // Install event filter
    for (auto screen : qGuiApp->screens())
        screen->installEventFilter(this);
    connect(qGuiApp, &QGuiApplication::screenAdded, this, [this](QScreen *screen) {
        screen->installEventFilter(this);
    });
    connect(qGuiApp, &QGuiApplication::screenRemoved, this, [this](QScreen *screen) {
        screen->removeEventFilter(this);
    });
#endif
}

ScreenCast::~ScreenCast()
{
    m_adaptor->deleteLater();
#ifdef SCREENCAST_ENABLED
    m_stream->deleteLater();
#endif
}

QWaylandCompositor *ScreenCast::compositor() const
{
    return m_compositor;
}

void ScreenCast::setCompositor(QWaylandCompositor *compositor)
{
    if (m_compositor == compositor)
        return;

    m_compositor = compositor;
    Q_EMIT compositorChanged();
}

bool ScreenCast::createStream(QScreen *screen)
{
#ifdef SCREENCAST_ENABLED
    if (!screen)
        return false;

    // Stop streaming
    if (m_streamingEnabled) {
        m_stream->stopStreaming();
        Liri::Platform::EglFSFunctions::disableScreenCapture(screen);
    }

    // Create the stream
    if (!m_stream->createStream(screen->geometry().size())) {
        qCWarning(lcScreenCast) << "Failed to create PipeWire stream!";
        return false;
    }

    // Enable streaming for the screen
    Liri::Platform::EglFSFunctions::enableScreenCapture(screen);

    return true;
#else
    Q_UNUSED(screen);
    qCWarning(lcScreenCast) << "Screen cast support disabled";
    return false;
#endif
}

#ifdef SCREENCAST_ENABLED
bool ScreenCast::eventFilter(QObject *obj, QEvent *event)
{
    if (event->type() == Liri::Platform::FrameCaptureEvent::registeredType()) {
        auto frameEvent = static_cast<Liri::Platform::FrameCaptureEvent *>(event);
        m_stream->recordFrame(frameEvent->capture.bits());
        return true;
    }

    return QObject::eventFilter(obj, event);
}
#endif

void ScreenCast::componentComplete()
{
    if (!compositor()) {
        for (QObject *p = parent(); p != nullptr; p = p->parent()) {
            if (auto c = qobject_cast<QWaylandCompositor *>(p)) {
                setCompositor(c);
                break;
            }
        }
    }
}
