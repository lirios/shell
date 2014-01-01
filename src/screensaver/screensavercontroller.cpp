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

#include <QtCore/QDebug>
#include <QtGui/QGuiApplication>
#include <QtGui/qpa/qplatformnativeinterface.h>

#include "screensavercontroller.h"
#include "screensaverview.h"

ScreenSaverController::ScreenSaverController(QObject *parent)
    : QObject(parent)
    , m_screensaver(nullptr)
{
}

ScreenSaverController::~ScreenSaverController()
{
    for (ScreenSaverView *view: m_windows) {
        if (m_windows.removeOne(view))
            view->deleteLater();
    }

    delete m_screensaver;
}

void ScreenSaverController::initialize()
{
    // Platform native interface
    QPlatformNativeInterface *native =
            QGuiApplication::platformNativeInterface();
    Q_ASSERT(native);

    // Get Wayland display
    struct ::wl_display *display = static_cast<struct ::wl_display *>(
                native->nativeResourceForIntegration("display"));
    Q_ASSERT(display);

    // Wayland registry
    struct ::wl_registry *registry = wl_display_get_registry(display);
    Q_ASSERT(registry);

    // Initialize interfaces
    m_screensaver = new QtWayland::wl_screensaver();
    wl_registry_add_listener(registry,
                             &ScreenSaverController::registryListener,
                             this);
}

void ScreenSaverController::run()
{
    // Go through the list of screens and create a window
    for (QScreen *screen: qApp->screens()) {
        // Call the screenRemoved slot when the screen goes away
        connect(screen, SIGNAL(destroyed(QObject *)),
                this, SLOT(screenRemoved(QObject*)));

        // Create a window for this screen
        screenAdded(screen);
    }

    // Create a window every time a screen is added
    connect(qApp, SIGNAL(screenAdded(QScreen*)),
            this, SLOT(screenAdded(QScreen*)));
}

void ScreenSaverController::screenAdded(QScreen *screen)
{
    qDebug() << "Creating window for screen" << screen->name();

    ScreenSaverView *view = new ScreenSaverView();
    view->setScreen(screen);
    view->setPosition(screen->geometry().topLeft());
    view->resize(view->size());
    view->show();
    setupWindow(view);
    m_windows.append(view);
}

void ScreenSaverController::screenRemoved(QObject *object)
{
    QScreen *screen = qobject_cast<QScreen *>(object);
    if (!screen)
        return;

    for (ScreenSaverView *view: m_windows) {
        if (view->screen() == screen) {
            m_windows.removeOne(view);
            view->deleteLater();
        }
    }
}

void ScreenSaverController::setupWindow(ScreenSaverView *window)
{
    if (!window) {
        qWarning() << "Couldn't setup screen saver window: invalid window";
        return;
    }
    if (!m_screensaver->isInitialized()) {
        qWarning() << "Couldn't setup screen saver window: interface not initialized";
        return;
    }

    QPlatformNativeInterface *native =
            QGuiApplication::platformNativeInterface();
    Q_ASSERT(native);

    struct ::wl_output *output = static_cast<struct ::wl_output *>(
                native->nativeResourceForScreen("output", window->screen()));
    struct ::wl_surface *surface = static_cast<struct ::wl_surface *>(
                native->nativeResourceForWindow("surface", window));
    m_screensaver->set_surface(output, surface);
}

void ScreenSaverController::handleGlobal(void *data,
                                         wl_registry *registry,
                                         uint32_t id,
                                         const char *interface,
                                         uint32_t version)
{
    Q_UNUSED(version);

    ScreenSaverController *self = static_cast<ScreenSaverController *>(data);
    if (!self) {
        qWarning() << "Unable to cast data to ScreenSaverController pointer in handleGlobal";
        return;
    }

    if (strcmp(interface, "wl_screensaver") == 0) {
        self->m_screensaver->init(registry, id);
        QMetaObject::invokeMethod(self, "run", Qt::QueuedConnection);
    }
}

void ScreenSaverController::handleGlobalRemove(void *data,
                                               wl_registry *registry,
                                               uint32_t name)
{
    Q_UNUSED(registry);
    Q_UNUSED(name);

    ScreenSaverController *self = static_cast<ScreenSaverController *>(data);
    if (!self) {
        qWarning() << "Unable to cast data to ScreenSaverController pointer in handleGlobal";
        return;
    }

    free(static_cast<void *>(self->m_screensaver->object()));
}

const struct wl_registry_listener ScreenSaverController::registryListener = {
    ScreenSaverController::handleGlobal,
    ScreenSaverController::handleGlobalRemove
};

#include "moc_screensavercontroller.cpp"
