// SPDX-FileCopyrightText: 2020 Pier Luigi Fiorini <pierluigi.fiorini@gmail.com>
//
// SPDX-License-Identifier: GPL-3.0-or-later

#include <QGuiApplication>
#include <QScreen>

#include <LiriPlatformHeaders/lirieglfsfunctions.h>

#include "logging.h"
#include "screencast.h"

ScreenCast::ScreenCast(QObject *parent)
    : QObject(parent)
{
    QGuiApplication::instance()->installEventFilter(this);
}

void ScreenCast::enable(QScreen *screen)
{
    if (QGuiApplication::platformName() != QLatin1String("liri")) {
        qCWarning(lcShell, "Cannot enable screencast: " \
                  "unsupported platform \"%s\", please use \"liri\"",
                  qPrintable(QGuiApplication::platformName()));
        return;
    }

    Liri::Platform::EglFSFunctions::enableScreenCast(screen);
}

void ScreenCast::disable(QScreen *screen)
{
    if (QGuiApplication::platformName() != QLatin1String("liri")) {
        qCWarning(lcShell, "Cannot disable screencast: " \
                  "unsupported platform \"%s\", please use \"liri\"",
                  qPrintable(QGuiApplication::platformName()));
        return;
    }

    Liri::Platform::EglFSFunctions::disableScreenCast(screen);
}

bool ScreenCast::eventFilter(QObject *obj, QEvent *event)
{
    if (event->type() == Liri::Platform::ScreenCastFrameEvent::registeredType()) {
        auto *e = static_cast<Liri::Platform::ScreenCastFrameEvent *>(event);
        Q_EMIT frameAvailable(e->screen, e->size, e->offset, e->drmFormat, e->modifier, e->numObjects);
        return true;
    } else if (event->type() == Liri::Platform::ScreenCastObjectEvent::registeredType()) {
        auto *e = static_cast<Liri::Platform::ScreenCastObjectEvent *>(event);
        Q_EMIT objectAvailable(e->screen, e->index, e->fd, e->size, e->offset, e->stride, e->planeIndex);
        return true;
    } else if (event->type() == Liri::Platform::ScreenCastReadyEvent::registeredType()) {
        auto *e = static_cast<Liri::Platform::ScreenCastReadyEvent *>(event);
        Q_EMIT captureReady(e->screen, e->tv_sec, e->tv_nsec);
        return true;
    }

    return QObject::eventFilter(obj, event);
}
