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

#include <QBuffer>
#include <QDBusConnection>
#include <QEventLoop>
#include <QPainter>
#include <QQuickItem>
#include <QQuickItemGrabResult>
#include <QQuickWindow>
#include <QWaylandCompositor>
#include <QWaylandOutput>
#include <QWaylandSurface>

#include "screenshooter.h"

Q_LOGGING_CATEGORY(lcScreenshooter, "liri.screenshooter", QtDebugMsg)

const QString dbusPath = QStringLiteral("/Screenshot");

/*
 * Screenshooter
 */

Screenshooter::Screenshooter(QObject *parent)
    : QObject(parent)
    , m_adaptor(new ScreenshooterAdaptor(this))
{
}

Screenshooter::~Screenshooter()
{
    delete m_adaptor;
}

QWaylandCompositor *Screenshooter::compositor() const
{
    return m_compositor;
}

void Screenshooter::setCompositor(QWaylandCompositor *compositor)
{
    if (m_compositor == compositor)
        return;

    m_compositor = compositor;
    Q_EMIT compositorChanged();
}

void Screenshooter::classBegin()
{
}

void Screenshooter::componentComplete()
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

/*
 * ScreenshooterAdaptor
 */

ScreenshooterAdaptor::ScreenshooterAdaptor(QObject *parent)
    : QObject(parent)
    , m_parent(qobject_cast<Screenshooter *>(parent))
{
    auto bus = QDBusConnection::sessionBus();
    bus.registerObject(dbusPath, this,
                       QDBusConnection::ExportScriptableContents);
}

ScreenshooterAdaptor::~ScreenshooterAdaptor()
{
    QDBusConnection::sessionBus().unregisterObject(dbusPath);
}

QByteArray ScreenshooterAdaptor::captureScreens(bool overlayCursor)
{
    setDelayedReply(true);

    if (!m_parent->m_compositor) {
        sendErrorReply(QDBusError::InternalError, QStringLiteral("Screenshooter::compositor not set"));
        return QByteArray();
    }

    // Create screenshot
    auto reply = message().createReply();
    auto screenshot = new Screenshot(Screenshot::CaptureScreens, reply);
    screenshot->m_compositor = m_parent->m_compositor;
    screenshot->m_overlayCursor = overlayCursor;
    Q_EMIT m_parent->captureRequested(screenshot);
    return QByteArray();
}

QByteArray ScreenshooterAdaptor::captureActiveWindow(bool overlayCursor, bool windowBorder)
{
    setDelayedReply(true);

    // Create screenshot
    auto reply = message().createReply();
    auto screenshot = new Screenshot(Screenshot::CaptureActiveWindow, reply);
    screenshot->m_compositor = m_parent->m_compositor;
    screenshot->m_overlayCursor = overlayCursor;
    screenshot->m_windowBorder = windowBorder;
    Q_EMIT m_parent->captureRequested(screenshot);
    return QByteArray();
}

QByteArray ScreenshooterAdaptor::captureWindow(bool overlayCursor, bool windowBorder)
{
    setDelayedReply(true);

    // Create screenshot
    auto reply = message().createReply();
    auto screenshot = new Screenshot(Screenshot::CaptureWindow, reply);
    screenshot->m_compositor = m_parent->m_compositor;
    screenshot->m_overlayCursor = overlayCursor;
    screenshot->m_windowBorder = windowBorder;
    Q_EMIT m_parent->captureRequested(screenshot);
    return QByteArray();

}

QByteArray ScreenshooterAdaptor::captureArea()
{
    setDelayedReply(true);

    // Create screenshot
    auto reply = message().createReply();
    auto screenshot = new Screenshot(Screenshot::CaptureArea, reply);
    screenshot->m_compositor = m_parent->m_compositor;
    Q_EMIT m_parent->captureRequested(screenshot);
    return QByteArray();
}

/*
 * Screenshot
 */

Screenshot::Screenshot(CaptureType captureType, const QDBusMessage &reply)
    : QObject()
    , m_captureType(captureType)
    , m_reply(reply)
{
}

Screenshot::CaptureType Screenshot::captureType() const
{
    return m_captureType;
}

bool Screenshot::overlayCursor() const
{
    return m_overlayCursor;
}

bool Screenshot::keepWindowBorder() const
{
    return m_windowBorder;
}

QImage Screenshot::grabItem(QQuickItem *item)
{
    if (!item)
        return QImage();

    // Capture item to an image
    QSharedPointer<QQuickItemGrabResult> result = item->grabToImage();
    if (result.isNull() || result->image().isNull()) {
        QEventLoop loop;
        connect(result.data(), &QQuickItemGrabResult::ready,
                &loop, &QEventLoop::quit);
        loop.exec();
    }

    return result->image();
}

void Screenshot::grabCursorItem(QQuickItem *item)
{
    m_cursorPosition = item->position();
    m_cursorImage = grabItem(item);
}

void Screenshot::grabScreens()
{
    if (m_captureType != Screenshot::CaptureScreens)
        return;

    if (!m_finalImage.isNull())
        return;

    QRect r;

    const auto outputs = m_compositor->outputs();

    // Calculate the final size
    for (auto output : outputs) {
        const bool enabled = output->property("enabled").toBool();

        if (!enabled)
            continue;

        r |= output->geometry();
    }

    m_finalImage = QImage(r.size(), QImage::Format_RGB32);
    QPainter painter(&m_finalImage);
    for (auto output : outputs) {
        auto window = qobject_cast<QQuickWindow *>(output->window());
        if (window)
            painter.drawImage(output->position(), window->grabWindow());
    }
}

void Screenshot::grabSurfaceItem(QQuickItem *item)
{
    if (!m_finalImage.isNull())
        return;

    m_finalImage = grabItem(item);
}

void Screenshot::save()
{
    // Add the mouse cursor
    if (m_overlayCursor)
        composeCursorImage(m_finalImage, m_cursorPosition, m_cursorImage);

    // Send reply back to the client
    QByteArray data;
    QBuffer buffer(&data);
    buffer.open(QBuffer::WriteOnly);
    m_finalImage.save(&buffer, "PNG");
    m_reply << data;
    QDBusConnection::sessionBus().send(m_reply);
}

void Screenshot::composeCursorImage(QImage &finalImage, const QPointF &position, const QImage &cursorImage)
{
    if (position.isNull() || cursorImage.isNull())
        return;

    QPainter painter(&finalImage);
    painter.drawImage(position, cursorImage);
}
