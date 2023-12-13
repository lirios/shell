// SPDX-FileCopyrightText: 2022 Pier Luigi Fiorini <pierluigi.fiorini@gmail.com>
//
// SPDX-License-Identifier: GPL-3.0-or-later

#include "extsessionlocksurfacev1.h"
#include "extsessionlocksurfacev1_p.h"
#include "lirishelllockscreenlogging.h"

// Both shell integration and QML plugins will share this global
// since the interface is in a dynamic library
typedef QHash<QWindow *, ExtSessionLockSurfaceV1 *> ExtSessionLockSurfaceV1Map;
Q_GLOBAL_STATIC(ExtSessionLockSurfaceV1Map, globalLockSurfaces)

ExtSessionLockSurfaceV1::ExtSessionLockSurfaceV1(QObject *parent)
    : QObject(parent)
    , d_ptr(new ExtSessionLockSurfaceV1Private())
{
}

ExtSessionLockSurfaceV1::~ExtSessionLockSurfaceV1()
{
    Q_D(ExtSessionLockSurfaceV1);
    globalLockSurfaces->remove(d->window);
}

bool ExtSessionLockSurfaceV1::isInitialized() const
{
    Q_D(const ExtSessionLockSurfaceV1);
    return d->initialized;
}

void ExtSessionLockSurfaceV1::initialize()
{
    Q_D(ExtSessionLockSurfaceV1);

    if (d->initialized)
        return;

    if (!d->window) {
        qCWarning(gLcLockScreen, "Window not assigned to ExtSessionLockSurfaceV1, failed to initialize");
        return;
    }

    d->initialized = true;
}

QWindow *ExtSessionLockSurfaceV1::window() const
{
    Q_D(const ExtSessionLockSurfaceV1);
    return d->window;
}

void ExtSessionLockSurfaceV1::setWindow(QWindow *window)
{
    Q_D(ExtSessionLockSurfaceV1);

    if (d->window == window)
        return;

    if (d->initialized) {
        qCWarning(gLcLockScreen, "Unable to change ExtSessionLockSurfaceV1::window after initialization");
        return;
    }

    d->window = window;
    emit windowChanged(d->window);

    if (!globalLockSurfaces->contains(d->window))
        globalLockSurfaces->insert(d->window, this);
}

ExtSessionLockSurfaceV1 *ExtSessionLockSurfaceV1::get(QWindow *window)
{
    return globalLockSurfaces->value(window, nullptr);
}

void ExtSessionLockSurfaceV1::componentComplete()
{
    if (isInitialized())
        return;

    // Find the window from the parent, if not specified
    if (!window()) {
        for (auto *p = parent(); p != nullptr; p = p->parent()) {
            if (auto *w = qobject_cast<QWindow *>(p)) {
                setWindow(w);
                break;
            }
        }
    }

    // Initialize
    initialize();
}
