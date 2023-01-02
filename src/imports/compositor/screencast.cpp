// SPDX-FileCopyrightText: 2020 Pier Luigi Fiorini <pierluigi.fiorini@gmail.com>
//
// SPDX-License-Identifier: GPL-3.0-or-later

#include <QGuiApplication>
#include <QScreen>

#include <LiriAuroraPlatformHeaders/lirieglfsfunctions.h>

#include "lirishellcompositorlogging.h"
#include "screencast.h"

ScreenCast::ScreenCast(QObject *parent)
    : QObject(parent)
{
    QGuiApplication::instance()->installEventFilter(this);
}

void ScreenCast::enable(QScreen *screen)
{
    if (QGuiApplication::platformName() != QLatin1String("liri")) {
        qCWarning(lcShellCompositor, "Cannot enable screencast: " \
                  "unsupported platform \"%s\", please use \"liri\"",
                  qPrintable(QGuiApplication::platformName()));
        return;
    }

    Aurora::PlatformSupport::EglFSFunctions::enableScreenCast(screen);
}

void ScreenCast::disable(QScreen *screen)
{
    if (QGuiApplication::platformName() != QLatin1String("liri")) {
        qCWarning(lcShellCompositor, "Cannot disable screencast: " \
                  "unsupported platform \"%s\", please use \"liri\"",
                  qPrintable(QGuiApplication::platformName()));
        return;
    }

    Aurora::PlatformSupport::EglFSFunctions::disableScreenCast(screen);
}

bool ScreenCast::eventFilter(QObject *obj, QEvent *event)
{
    if (event->type() == Aurora::PlatformSupport::ScreenCastFrameEvent::registeredType()) {
        auto *e = static_cast<Aurora::PlatformSupport::ScreenCastFrameEvent *>(event);
        Q_EMIT frameAvailable(e->screen, e->size, e->offset, e->drmFormat, e->modifier, e->numObjects);
        return true;
    } else if (event->type() == Aurora::PlatformSupport::ScreenCastObjectEvent::registeredType()) {
        auto *e = static_cast<Aurora::PlatformSupport::ScreenCastObjectEvent *>(event);
        Q_EMIT objectAvailable(e->screen, e->index, e->fd, e->size, e->offset, e->stride, e->planeIndex);
        return true;
    } else if (event->type() == Aurora::PlatformSupport::ScreenCastReadyEvent::registeredType()) {
        auto *e = static_cast<Aurora::PlatformSupport::ScreenCastReadyEvent *>(event);
        Q_EMIT captureReady(e->screen, e->tv_sec, e->tv_nsec);
        return true;
    }

    return QObject::eventFilter(obj, event);
}
