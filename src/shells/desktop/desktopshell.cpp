/****************************************************************************
 * This file is part of Hawaii Shell.
 *
 * Copyright (C) 2012-2013 Pier Luigi Fiorini <pierluigi.fiorini@gmail.com>
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
#include <QtGui/QScreen>
#include <QtQml/QQmlEngine>
#include <QtQml/QQmlComponent>
#include <QtQml/QQmlContext>

#include <qpa/qplatformnativeinterface.h>

#include "applicationiconprovider.h"
#include "cmakedirs.h"
#include "registration.h"
#include "desktopshell.h"
#include "desktopshell_p.h"
#include "notificationsdaemon.h"
#include "notificationwindow.h"
#include "notificationimageprovider.h"
#include "shellui.h"
#include "shellwindow.h"
#include "shortcut_p.h"
#include "window.h"
#include "window_p.h"
#include "workspace.h"
#include "workspace_p.h"
#include "keybinding.h"
#include "keybinding_p.h"
#include "servicefactory.h"

/*
 * ShellImpl
 */

DesktopShellImpl::DesktopShellImpl(DesktopShellPrivate *parent)
    : QtWayland::hawaii_desktop_shell()
    , m_parent(parent)
{
}

void DesktopShellImpl::hawaii_desktop_shell_loaded()
{
    // Create shell windows
    QMetaObject::invokeMethod(m_parent->q_ptr, "create");
}

void DesktopShellImpl::hawaii_desktop_shell_prepare_lock_surface()
{
    // Create the lock screen only for the primary screen
    // TODO: Actually we should create one for each screen but this
    // requires protocol changes
    foreach (ShellUi *shellUi, m_parent->shellWindows) {
        if (shellUi->screen() == QGuiApplication::primaryScreen()) {
            QMetaObject::invokeMethod(shellUi, "createLockScreenWindow");
            return;
        }
    }
}

void DesktopShellImpl::hawaii_desktop_shell_grab_cursor(uint32_t cursor)
{
    QCursor qcursor;

    switch (cursor) {
    case HAWAII_DESKTOP_SHELL_CURSOR_NONE:
        break;
    case HAWAII_DESKTOP_SHELL_CURSOR_BUSY:
        qcursor.setShape(Qt::BusyCursor);
        break;
    case HAWAII_DESKTOP_SHELL_CURSOR_MOVE:
        qcursor.setShape(Qt::DragMoveCursor);
        break;
    case HAWAII_DESKTOP_SHELL_CURSOR_RESIZE_TOP:
        qcursor.setShape(Qt::SizeVerCursor);
        break;
    case HAWAII_DESKTOP_SHELL_CURSOR_RESIZE_BOTTOM:
        qcursor.setShape(Qt::SizeVerCursor);
        break;
    case HAWAII_DESKTOP_SHELL_CURSOR_RESIZE_LEFT:
        qcursor.setShape(Qt::SizeHorCursor);
        break;
    case HAWAII_DESKTOP_SHELL_CURSOR_RESIZE_RIGHT:
        qcursor.setShape(Qt::SizeHorCursor);
        break;
    case HAWAII_DESKTOP_SHELL_CURSOR_RESIZE_TOP_LEFT:
        qcursor.setShape(Qt::SizeFDiagCursor);
        break;
    case HAWAII_DESKTOP_SHELL_CURSOR_RESIZE_TOP_RIGHT:
        qcursor.setShape(Qt::SizeBDiagCursor);
        break;
    case HAWAII_DESKTOP_SHELL_CURSOR_RESIZE_BOTTOM_LEFT:
        qcursor.setShape(Qt::SizeBDiagCursor);
        break;
    case HAWAII_DESKTOP_SHELL_CURSOR_RESIZE_BOTTOM_RIGHT:
        qcursor.setShape(Qt::SizeFDiagCursor);
        break;
    case HAWAII_DESKTOP_SHELL_CURSOR_ARROW:
    default:
        qcursor.setShape(Qt::ArrowCursor);
        break;
    }

    foreach (ShellUi *shellUi, m_parent->shellWindows)
        QMetaObject::invokeMethod(shellUi->grabWindow(), "setGrabCursor",
                                  Qt::QueuedConnection,
                                  Q_ARG(QCursor, qcursor));
}

void DesktopShellImpl::hawaii_desktop_shell_window_mapped(struct ::hawaii_desktop_shell *object,
                                                          struct ::hawaii_window *id,
                                                          const QString &title,
                                                          const QString &identifier,
                                                          int32_t state)
{
    // Create a client window representation
    Window *window = new Window(title, identifier, wlStateConvert(state));
    window->d_ptr->init(id);

    window->moveToThread(QCoreApplication::instance()->thread());
    m_parent->windows.append(window);
    m_parent->emitWindowAdded(window);
}

void DesktopShellImpl::hawaii_desktop_shell_workspace_added(struct ::hawaii_desktop_shell *object,
                                                            struct ::hawaii_workspace *id,
                                                            int32_t active)
{
    Workspace *workspace = new Workspace(active != 0);
    workspace->d_ptr->init(id);

    workspace->moveToThread(QCoreApplication::instance()->thread());
    m_parent->workspaces.append(workspace);
    m_parent->emitWorkspaceAdded(m_parent->workspaces.indexOf(workspace));
}

/*
 * DesktopShellPrivate
 */

DesktopShellPrivate::DesktopShellPrivate(DesktopShell *parent)
    : q_ptr(parent)
    , windowsMinimized(false)
{
    // Start counting how much time we need to start up :)
    elapsedTimer.start();

    // We need windows with alpha buffer
    QQuickWindow::setDefaultAlphaBuffer(true);

    // Create QML engine
    engine = new QQmlEngine(parent);
    engine->rootContext()->setContextProperty("Shell", parent);

    // Register image providers
    engine->addImageProvider("appicon", new ApplicationIconProvider);
    engine->addImageProvider("notifications", new NotificationImageProvider);

    // Register QML types and factories
    registerQmlTypes();
    registerFactories();

    // Platform native interface
    QPlatformNativeInterface *native =
            QGuiApplication::platformNativeInterface();
    Q_ASSERT(native);

    // Get Wayland display
    display = static_cast<struct wl_display *>(
                native->nativeResourceForIntegration("display"));
    Q_ASSERT(display);

    // Display file descriptor
    int fd = wl_display_get_fd(display);
    Q_ASSERT(fd > -1);
    qDebug() << "Wayland display socket:" << fd;

    // Wayland registry
    registry = wl_display_get_registry(display);
    Q_ASSERT(registry);

    // Initialize interfaces
    shell = new DesktopShellImpl(this);
    notifications = new QtWayland::wl_notification_daemon();
    wl_registry_add_listener(registry,
                             &DesktopShellPrivate::registryListener,
                             this);
}

DesktopShellPrivate::~DesktopShellPrivate()
{
    wl_notification_daemon_destroy(notifications->object());
    delete notifications;

    hawaii_desktop_shell_destroy(shell->object());
    delete shell;

    qDeleteAll(workspaces);
    qDeleteAll(shellWindows);
    qDeleteAll(services);
}

void DesktopShellPrivate::emitWindowAdded(Window *window)
{
    Q_Q(DesktopShell);

    QObject::connect(window, SIGNAL(unmapped(Window*)),
                     q, SLOT(windowUnmapped(Window*)));
    QObject::connect(window, &Window::activeChanged, [=](bool active) {
        Q_EMIT q->windowActivated(window);
    });

    Q_EMIT q->windowsChanged();
}

void DesktopShellPrivate::emitWorkspaceAdded(int num)
{
    Q_Q(DesktopShell);

    Q_EMIT q->workspaceAdded(num);
    Q_EMIT q->workspacesChanged();

    Workspace *workspace = workspaces.at(num);
    q->connect(workspace, &Workspace::activeChanged, [=](bool active){
        Q_EMIT q->workspaceSwitched(workspace);
    });
}

void DesktopShellPrivate::handleGlobal(void *data,
                                       wl_registry *registry,
                                       uint32_t id,
                                       const char *interface,
                                       uint32_t version)
{
    Q_UNUSED(version);

    DesktopShellPrivate *self = static_cast<DesktopShellPrivate *>(data);
    if (!self) {
        qWarning() << "Unable to cast data to DesktopShellPrivate pointer";
        return;
    }

    if (strcmp(interface, "hawaii_desktop_shell") == 0) {
        self->shell->init(registry, id);
    } else if (strcmp(interface, "wl_notification_daemon") == 0) {
        self->notifications->init(registry, id);

        // Start the notifications daemon and connect to the session bus
        NotificationsDaemon *daemon = NotificationsDaemon::instance();
        QMetaObject::invokeMethod(daemon, "connectOnDBus");
    }
}

const struct wl_registry_listener DesktopShellPrivate::registryListener = {
    DesktopShellPrivate::handleGlobal
};

/*
 * DesktopShell
 */

Q_GLOBAL_STATIC(DesktopShell, s_desktopShell)

DesktopShell::DesktopShell()
    : QObject()
    , d_ptr(new DesktopShellPrivate(this))
{
}

DesktopShell::~DesktopShell()
{
    delete d_ptr;
}

DesktopShell *DesktopShell::instance()
{
    return s_desktopShell();
}

void DesktopShell::create()
{
    Q_D(DesktopShell);

    // Add configured workspaces
    // TODO: Add as many workspaces as specified by the settings
    addWorkspaces(4);

    // Bind Meta-D to toggle windows
    KeyBinding *binding = addKeyBinding(KEY_D, MODIFIER_SUPER);
    connect(binding, &KeyBinding::triggered, []() {
        DesktopShell::instance()->toggleWindows();
    });

    // Create shell windows
    foreach (QScreen *screen, QGuiApplication::screens()) {
        qDebug() << "--- Screen" << screen->name() << screen->geometry();

        ShellUi *ui = new ShellUi(d->engine, screen, this);
        d->shellWindows.append(ui);
    }

    // Wait until all user interface elements for all screens are ready
    while (QCoreApplication::hasPendingEvents())
        QCoreApplication::processEvents();

    // Shell user interface is ready, tell the compositor to fade in
    d->shell->desktop_ready();
    qDebug() << "Shell is now ready and took" << d->elapsedTimer.elapsed() << "ms";
}

QQmlEngine *DesktopShell::engine() const
{
    Q_D(const DesktopShell);
    return d->engine;
}

QObject *DesktopShell::service(const QString &name)
{
    Q_D(DesktopShell);

    // Get an already created service
    QObject *service = d->services.value(name);
    if (service)
        return service;

    // If we can't find it just create it
    service = ServiceFactory::createService(name);
    d->services[name] = service;
    return service;
}

KeyBinding *DesktopShell::addKeyBinding(quint32 key, quint32 modifiers)
{
    Q_D(DesktopShell);

    KeyBinding *keyBinding = new KeyBinding(key, modifiers, this);
    keyBinding->d_ptr->init(d->shell->add_key_binding(key, modifiers));
    d->keyBindings.append(keyBinding);
    return keyBinding;
}

void DesktopShell::setAvailableGeometry(QScreen *screen, const QRect &geometry)
{
    Q_D(DesktopShell);

    QPlatformNativeInterface *native = QGuiApplication::platformNativeInterface();
    wl_output *output = static_cast<struct wl_output *>(
                native->nativeResourceForScreen("output", screen));
    d->shell->set_available_geometry(output,
                                     geometry.x(), geometry.y(),
                                     geometry.width(), geometry.height());
}

QList<ShellUi *> DesktopShell::shellWindows() const
{
    Q_D(const DesktopShell);
    return d->shellWindows;
}

QQmlListProperty<Window> DesktopShell::windows()
{
    return QQmlListProperty<Window>(this, 0, windowsCount, windowAt);
}

QQmlListProperty<Workspace> DesktopShell::workspaces()
{
    return QQmlListProperty<Workspace>(this, 0, workspacesCount, workspaceAt);
}

void DesktopShell::minimizeWindows()
{
    Q_D(DesktopShell);
    d->shell->minimize_windows();
    d->windowsMinimized = true;
}

void DesktopShell::restoreWindows()
{
    Q_D(DesktopShell);
    d->shell->restore_windows();
    d->windowsMinimized = false;
}

void DesktopShell::toggleWindows()
{
    Q_D(DesktopShell);
    if (d->windowsMinimized)
        restoreWindows();
    else
        minimizeWindows();
}

void DesktopShell::addWorkspace()
{
    Q_D(DesktopShell);
    d->shell->add_workspace();
}

void DesktopShell::addWorkspaces(int num)
{
    Q_D(DesktopShell);

    while (num--)
        d->shell->add_workspace();
}

void DesktopShell::removeWorkspace(int num)
{
    Q_D(DesktopShell);

    Workspace *workspace = d->workspaces.takeAt(num);
    if (workspace) {
        Q_EMIT workspaceRemoved(num);
        delete workspace;
    }
}

void DesktopShell::selectWorkspace(Workspace *workspace)
{
    Q_D(DesktopShell);
    d->shell->select_workspace(workspace->d_ptr->object());
}

int DesktopShell::windowsCount(QQmlListProperty<Window> *p)
{
    DesktopShell *shell = static_cast<DesktopShell *>(p->object);
    return shell->d_ptr->windows.size();
}

Window *DesktopShell::windowAt(QQmlListProperty<Window> *p, int index)
{
    DesktopShell *shell = static_cast<DesktopShell *>(p->object);
    return shell->d_ptr->windows.at(index);
}

int DesktopShell::workspacesCount(QQmlListProperty<Workspace> *p)
{
    DesktopShell *shell = static_cast<DesktopShell *>(p->object);
    return shell->d_ptr->workspaces.size();
}

Workspace *DesktopShell::workspaceAt(QQmlListProperty<Workspace> *p, int index)
{
    DesktopShell *shell = static_cast<DesktopShell *>(p->object);
    return shell->d_ptr->workspaces.at(index);
}

void DesktopShell::windowUnmapped(Window *window)
{
    Q_D(DesktopShell);

    d->windows.removeOne(window);
    Q_EMIT windowsChanged();
}

#include "moc_desktopshell.cpp"
