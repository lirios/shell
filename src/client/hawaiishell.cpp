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
#include "elementfactory.h"
#include "hawaiishell.h"
#include "hawaiishell_p.h"
#include "notificationsdaemon.h"
#include "notificationwindow.h"
#include "notificationimageprovider.h"
#include "keybinding.h"
#include "keybinding_p.h"
#include "registration.h"
#include "servicefactory.h"
#include "shellui.h"
#include "shellwindow.h"
#include "shortcut_p.h"
#include "window.h"
#include "window_p.h"
#include "workspace.h"
#include "workspace_p.h"

/*
 * ShellImpl
 */

HawaiiShellImpl::HawaiiShellImpl(HawaiiShellPrivate *parent)
    : QtWayland::wl_hawaii_shell()
    , m_parent(parent)
{
}

void HawaiiShellImpl::hawaii_shell_loaded()
{
    // Create shell windows
    QMetaObject::invokeMethod(m_parent->q_ptr, "create");
}

void HawaiiShellImpl::hawaii_shell_prepare_lock_surface()
{
    // Create the lock screen
    QMetaObject::invokeMethod(m_parent->uiController, "createLockScreenWindow");
}

void HawaiiShellImpl::hawaii_shell_grab_cursor(uint32_t cursor)
{
    QCursor qcursor;

    switch (cursor) {
    case WL_HAWAII_SHELL_CURSOR_NONE:
        break;
    case WL_HAWAII_SHELL_CURSOR_BUSY:
        qcursor.setShape(Qt::BusyCursor);
        break;
    case WL_HAWAII_SHELL_CURSOR_MOVE:
        qcursor.setShape(Qt::DragMoveCursor);
        break;
    case WL_HAWAII_SHELL_CURSOR_RESIZE_TOP:
        qcursor.setShape(Qt::SizeVerCursor);
        break;
    case WL_HAWAII_SHELL_CURSOR_RESIZE_BOTTOM:
        qcursor.setShape(Qt::SizeVerCursor);
        break;
    case WL_HAWAII_SHELL_CURSOR_RESIZE_LEFT:
        qcursor.setShape(Qt::SizeHorCursor);
        break;
    case WL_HAWAII_SHELL_CURSOR_RESIZE_RIGHT:
        qcursor.setShape(Qt::SizeHorCursor);
        break;
    case WL_HAWAII_SHELL_CURSOR_RESIZE_TOP_LEFT:
        qcursor.setShape(Qt::SizeFDiagCursor);
        break;
    case WL_HAWAII_SHELL_CURSOR_RESIZE_TOP_RIGHT:
        qcursor.setShape(Qt::SizeBDiagCursor);
        break;
    case WL_HAWAII_SHELL_CURSOR_RESIZE_BOTTOM_LEFT:
        qcursor.setShape(Qt::SizeBDiagCursor);
        break;
    case WL_HAWAII_SHELL_CURSOR_RESIZE_BOTTOM_RIGHT:
        qcursor.setShape(Qt::SizeFDiagCursor);
        break;
    case WL_HAWAII_SHELL_CURSOR_ARROW:
    default:
        qcursor.setShape(Qt::ArrowCursor);
        break;
    }

    QMetaObject::invokeMethod(m_parent->uiController->grabWindow(), "setGrabCursor",
                              Qt::QueuedConnection,
                              Q_ARG(QCursor, qcursor));
}

void HawaiiShellImpl::hawaii_shell_window_mapped(struct ::wl_hawaii_shell *object,
                                                 struct ::wl_hawaii_window *id,
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

void HawaiiShellImpl::hawaii_shell_window_switching_started()
{
    m_parent->emitWindowSwitching(true);
}

void HawaiiShellImpl::hawaii_shell_window_switching_finished()
{
    m_parent->emitWindowSwitching(false);
}

void HawaiiShellImpl::hawaii_shell_window_switched(struct ::wl_hawaii_window *window)
{
    for (int i = 0; i < m_parent->windows.size(); i++) {
        Window *w = m_parent->windows.at(i);
        if (w->d_ptr->object() == window) {
            m_parent->emitWindowSwitchingNext(w);
            break;
        }
    }
}

void HawaiiShellImpl::hawaii_shell_workspace_added(struct ::wl_hawaii_shell *object,
                                                   struct ::wl_hawaii_workspace *id,
                                                   int32_t active)
{
    Workspace *workspace = new Workspace(active != 0);
    workspace->d_ptr->init(id);

    workspace->moveToThread(QCoreApplication::instance()->thread());
    m_parent->workspaces.append(workspace);
    m_parent->emitWorkspaceAdded(m_parent->workspaces.indexOf(workspace));
}

/*
 * HawaiiShellPrivate
 */

HawaiiShellPrivate::HawaiiShellPrivate(HawaiiShell *parent)
    : q_ptr(parent)
    , registrar(0)
    , uiController(0)
    , windowsMinimized(false)
{
    // Start counting how much time we need to start up :)
    elapsedTimer.start();

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
    shell = new HawaiiShellImpl(this);
    notifications = new QtWayland::wl_notification_daemon();
    wl_registry_add_listener(registry,
                             &HawaiiShellPrivate::registryListener,
                             this);
}

HawaiiShellPrivate::~HawaiiShellPrivate()
{
    wl_notification_daemon_destroy(notifications->object());
    delete notifications;

    wl_hawaii_shell_destroy(shell->object());
    delete shell;

    qDeleteAll(workspaces);

    ElementFactory::cleanupElements();
}

void HawaiiShellPrivate::emitWindowAdded(Window *window)
{
    Q_Q(HawaiiShell);

    QObject::connect(window, SIGNAL(unmapped(Window*)),
                     q, SLOT(windowUnmapped(Window*)));
    QObject::connect(window, &Window::activeChanged, [=](bool active) {
        Q_EMIT q->windowActivated(window);
    });

    Q_EMIT q->windowsChanged();
}

void HawaiiShellPrivate::emitWindowSwitching(bool started)
{
    Q_Q(HawaiiShell);

    if (started)
        Q_EMIT q->windowSwitchingStarted();
    else
        Q_EMIT q->windowSwitchingFinished();
}

void HawaiiShellPrivate::emitWindowSwitchingNext(Window *window)
{
    Q_Q(HawaiiShell);
    Q_EMIT q->windowSwitchingNext(window);
}

void HawaiiShellPrivate::emitWorkspaceAdded(int num)
{
    Q_Q(HawaiiShell);

    Q_EMIT q->workspaceAdded(num);
    Q_EMIT q->workspacesChanged();

    Workspace *workspace = workspaces.at(num);
    q->connect(workspace, &Workspace::activeChanged, [=](bool active){
        Q_EMIT q->workspaceSwitched(workspace);
    });
}

void HawaiiShellPrivate::handleGlobal(void *data,
                                      wl_registry *registry,
                                      uint32_t id,
                                      const char *interface,
                                      uint32_t version)
{
    Q_UNUSED(version);

    HawaiiShellPrivate *self = static_cast<HawaiiShellPrivate *>(data);
    if (!self) {
        qWarning() << "Unable to cast data to HawaiiShellPrivate pointer in handleGlobal";
        return;
    }

    if (strcmp(interface, "wl_hawaii_shell") == 0) {
        self->shell->init(registry, id);
    } else if (strcmp(interface, "wl_notification_daemon") == 0) {
        self->notifications->init(registry, id);

        // Start the notifications daemon and connect to the session bus
        NotificationsDaemon *daemon = NotificationsDaemon::instance();
        QMetaObject::invokeMethod(daemon, "connectOnDBus");
    }
}

void HawaiiShellPrivate::handleGlobalRemove(void *data,
                                            wl_registry *registry,
                                            uint32_t name)
{
    Q_UNUSED(name);

    HawaiiShellPrivate *self = static_cast<HawaiiShellPrivate *>(data);
    if (!self) {
        qWarning() << "Unable to cast data to HawaiiShellPrivate pointer in handleGlobalRemove";
        return;
    }

    // TODO: Delete objects
}

const struct wl_registry_listener HawaiiShellPrivate::registryListener = {
    HawaiiShellPrivate::handleGlobal,
    HawaiiShellPrivate::handleGlobalRemove
};

/*
 * HawaiiShell
 */

Q_GLOBAL_STATIC(HawaiiShell, s_desktopShell)

HawaiiShell::HawaiiShell()
    : QObject()
    , d_ptr(new HawaiiShellPrivate(this))
{
}

HawaiiShell::~HawaiiShell()
{
    delete d_ptr;
}

HawaiiShell *HawaiiShell::instance()
{
    return s_desktopShell();
}

void HawaiiShell::create()
{
    Q_D(HawaiiShell);

    // Add configured workspaces
    // TODO: Add as many workspaces as specified by the settings
    addWorkspaces(4);

    // Bind Meta-D to toggle windows
    KeyBinding *binding = addKeyBinding(KEY_D, MODIFIER_SUPER);
    connect(binding, &KeyBinding::triggered, []() {
        HawaiiShell::instance()->toggleWindows();
    });

    // We need windows with alpha buffer
    QQuickWindow::setDefaultAlphaBuffer(true);

    // Create QML engine
    d->engine = new QQmlEngine(this);
    d->engine->rootContext()->setContextProperty("Shell", this);

    // Register image providers
    d->engine->addImageProvider("appicon", new ApplicationIconProvider);
    d->engine->addImageProvider("notifications", new NotificationImageProvider);

    // Register QML types and factories
    Registration::registerQmlTypes();
    Registration::registerFactories();

    // Search elements
    ElementFactory::searchElements();

    // Register daemons and singletons
    d->registrar = new QQmlComponent(d->engine, QUrl("qrc:/qml/Registrar.qml"), this);
    if (d->registrar->status() != QQmlComponent::Ready)
        qFatal("Unable to register daemons and singletons: %s",
               qPrintable(d->registrar->errorString()));
    (void)d->registrar->create();

    // Create the UI controller
    d->uiController = new ShellUi(d->engine, this);
    d->engine->rootContext()->setContextProperty("Ui", d->uiController);

    // Create shell screens
    foreach (QScreen *screen, QGuiApplication::screens()) {
        qDebug() << "--- Screen" << screen->name() << screen->geometry();
        d->uiController->loadScreen(screen);
    }

    // Wait until all user interface elements for all screens are ready
    while (QCoreApplication::hasPendingEvents())
        QCoreApplication::processEvents();

    // Shell user interface is ready, tell the compositor to fade in
    d->shell->desktop_ready();
    qDebug() << "Shell is now ready, elapsed time:" << d->elapsedTimer.elapsed() << "ms";
}

QQmlEngine *HawaiiShell::engine() const
{
    Q_D(const HawaiiShell);
    return d->engine;
}

QObject *HawaiiShell::service(const QString &name)
{
    Q_D(HawaiiShell);

    // Get an already created service
    QObject *service = d->services.value(name);
    if (service)
        return service;

    // If we can't find it just create it
    service = ServiceFactory::createService(name);
    d->services[name] = service;
    return service;
}

KeyBinding *HawaiiShell::addKeyBinding(quint32 key, quint32 modifiers)
{
    Q_D(HawaiiShell);

    KeyBinding *keyBinding = new KeyBinding(key, modifiers, this);
    keyBinding->d_ptr->init(d->shell->add_key_binding(key, modifiers));
    d->keyBindings.append(keyBinding);
    return keyBinding;
}

void HawaiiShell::setAvailableGeometry(QScreen *screen, const QRect &geometry)
{
    Q_D(HawaiiShell);

    QPlatformNativeInterface *native = QGuiApplication::platformNativeInterface();
    wl_output *output = static_cast<struct wl_output *>(
                native->nativeResourceForScreen("output", screen));
    d->shell->set_available_geometry(output,
                                     geometry.x(), geometry.y(),
                                     geometry.width(), geometry.height());
}

ShellUi *HawaiiShell::uiController() const
{
    Q_D(const HawaiiShell);
    return d->uiController;
}

QQmlListProperty<Window> HawaiiShell::windows()
{
    return QQmlListProperty<Window>(this, 0, windowsCount, windowAt);
}

QQmlListProperty<Workspace> HawaiiShell::workspaces()
{
    return QQmlListProperty<Workspace>(this, 0, workspacesCount, workspaceAt);
}

void HawaiiShell::minimizeWindows()
{
    Q_D(HawaiiShell);
    d->shell->minimize_windows();
    d->windowsMinimized = true;
}

void HawaiiShell::restoreWindows()
{
    Q_D(HawaiiShell);
    d->shell->restore_windows();
    d->windowsMinimized = false;
}

void HawaiiShell::toggleWindows()
{
    Q_D(HawaiiShell);
    if (d->windowsMinimized)
        restoreWindows();
    else
        minimizeWindows();
}

void HawaiiShell::addWorkspace()
{
    Q_D(HawaiiShell);
    d->shell->add_workspace();
}

void HawaiiShell::addWorkspaces(int num)
{
    Q_D(HawaiiShell);

    while (num--)
        d->shell->add_workspace();
}

void HawaiiShell::removeWorkspace(int num)
{
    Q_D(HawaiiShell);

    Workspace *workspace = d->workspaces.takeAt(num);
    if (workspace) {
        Q_EMIT workspaceRemoved(num);
        delete workspace;
    }
}

void HawaiiShell::selectWorkspace(Workspace *workspace)
{
    Q_D(HawaiiShell);
    d->shell->select_workspace(workspace->d_ptr->object());
}

int HawaiiShell::windowsCount(QQmlListProperty<Window> *p)
{
    HawaiiShell *shell = static_cast<HawaiiShell *>(p->object);
    return shell->d_ptr->windows.size();
}

Window *HawaiiShell::windowAt(QQmlListProperty<Window> *p, int index)
{
    HawaiiShell *shell = static_cast<HawaiiShell *>(p->object);
    return shell->d_ptr->windows.at(index);
}

int HawaiiShell::workspacesCount(QQmlListProperty<Workspace> *p)
{
    HawaiiShell *shell = static_cast<HawaiiShell *>(p->object);
    return shell->d_ptr->workspaces.size();
}

Workspace *HawaiiShell::workspaceAt(QQmlListProperty<Workspace> *p, int index)
{
    HawaiiShell *shell = static_cast<HawaiiShell *>(p->object);
    return shell->d_ptr->workspaces.at(index);
}

void HawaiiShell::windowUnmapped(Window *window)
{
    Q_D(HawaiiShell);

    d->windows.removeOne(window);
    Q_EMIT windowsChanged();
}

#include "moc_hawaiishell.cpp"
