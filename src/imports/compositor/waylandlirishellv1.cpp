// SPDX-FileCopyrightText: 2019-2022 Pier Luigi Fiorini <pierluigi.fiorini@gmail.com>
//
// SPDX-License-Identifier: GPL-3.0-or-later

#include <QFile>

#include <LiriAuroraCompositor/WaylandCompositor>
#include <LiriAuroraCompositor/WaylandPointer>
#include <LiriAuroraCompositor/WaylandSeat>
#include <LiriAuroraCompositor/WaylandSurface>
#include <LiriAuroraCompositor/private/aurorawaylandoutput_p.h>

#include "waylandlirishellv1_p.h"

#include <wayland-server-protocol.h>

Q_LOGGING_CATEGORY(gLcLiriShellV1, "liri.shell.lirishellv1")

/*
 * WaylandLiriShellV1Private
 */

WaylandLiriShellV1Private::WaylandLiriShellV1Private(WaylandLiriShellV1 *qq)
    : WaylandCompositorExtensionPrivate(qq)
    , PrivateServer::zliri_shell_v1()
{
}

WaylandLiriShellV1Private *WaylandLiriShellV1Private::get(WaylandLiriShellV1 *shell)
{
    return shell->d_func();
}

void WaylandLiriShellV1Private::zliri_shell_v1_bind_resource(Resource *r)
{
    // Make sure only an authorized client can bind
    pid_t pid;
    wl_client_get_credentials(r->client(), &pid, nullptr, nullptr);
    QFile file(QString::asprintf("/proc/%d/cmdline", pid));
    if (file.open(QFile::ReadOnly)) {
        QTextStream stream(&file);
        QString data = stream.readAll();
        QStringList args = data.split(QLatin1Char('\0'));
        file.close();

        if (!args.first().endsWith(QStringLiteral("liri-shell-helper"))) {
            wl_resource_post_error(r->handle, WL_DISPLAY_ERROR_IMPLEMENTATION,
                                   "unauthorized client program");
            return;
        }
    } else {
        wl_resource_post_error(r->handle, WL_DISPLAY_ERROR_IMPLEMENTATION,
                               "unauthorized client program");
        return;
    }

    // Client can bind only once
    if (resource())
        wl_resource_post_error(r->handle,
                               WL_DISPLAY_ERROR_INVALID_OBJECT,
                               "client can bind only once");
}

void WaylandLiriShellV1Private::zliri_shell_v1_bind_shortcut(Resource *resource,
                                                             uint32_t id,
                                                             const QString &sequence)
{
    Q_Q(WaylandLiriShellV1);

    auto *compositor = static_cast<WaylandCompositor *>(q->extensionContainer());

    auto *shortcut = new WaylandLiriShortcutV1(sequence, compositor, q);

    auto ver = qMin(1, WaylandLiriShortcutV1Private::interfaceVersion());
    WaylandLiriShortcutV1Private::get(shortcut)->init(resource->client(), id, ver);

    Q_EMIT q->shortcutBound(shortcut);
}

void WaylandLiriShellV1Private::zliri_shell_v1_ready(Resource *resource, struct ::wl_resource *outputResource)
{
    Q_UNUSED(resource)

    Q_Q(WaylandLiriShellV1);

    auto *output = WaylandOutput::fromResource(outputResource);
    Q_ASSERT(output);
    qWarning()<<"@@@@@@@@@@@@@@@@"<<output;
    emit q->ready(output);
}

void WaylandLiriShellV1Private::zliri_shell_v1_terminate(Resource *resource)
{
    Q_UNUSED(resource);

    Q_Q(WaylandLiriShellV1);
    Q_EMIT q->terminateRequested();
}

/*
 * WaylandLiriShellV1
 */

WaylandLiriShellV1::WaylandLiriShellV1()
    : WaylandCompositorExtensionTemplate<WaylandLiriShellV1>(*new WaylandLiriShellV1Private(this))
{
}

WaylandLiriShellV1::WaylandLiriShellV1(Aurora::Compositor::WaylandCompositor *compositor)
    : WaylandCompositorExtensionTemplate<WaylandLiriShellV1>(compositor, *new WaylandLiriShellV1Private(this))
{
}

WaylandLiriShellV1::~WaylandLiriShellV1()
{
}

void WaylandLiriShellV1::initialize()
{
    Q_D(WaylandLiriShellV1);

    WaylandCompositorExtension::initialize();
    WaylandCompositor *compositor = static_cast<WaylandCompositor *>(extensionContainer());
    if (!compositor) {
        qCWarning(gLcLiriShellV1) << "Failed to find WaylandCompositor when initializing WaylandLiriShell";
        return;
    }
    d->init(compositor->display(), 1);
}

void WaylandLiriShellV1::showPanel(WaylandOutput *output)
{
    Q_D(WaylandLiriShellV1);

    auto *outputResource = WaylandOutputPrivate::get(output)->resource()->handle;

    const auto values = d->resourceMap().values();
    for (auto *resource : values)
        d->send_show_panel(resource->handle, outputResource);
}

void WaylandLiriShellV1::requestLogout()
{
    Q_D(WaylandLiriShellV1);

    const auto values = d->resourceMap().values();
    for (auto *resource : values)
        d->send_logout_requested(resource->handle);
}

void WaylandLiriShellV1::requestShutdown()
{
    Q_D(WaylandLiriShellV1);

    const auto values = d->resourceMap().values();
    for (auto *resource : values)
        d->send_shutdown_requested(resource->handle);
}

void WaylandLiriShellV1::sendQuit()
{
    Q_D(WaylandLiriShellV1);

    const auto values = d->resourceMap().values();
    for (auto *resource : values)
        d->send_quit(resource->handle);
}

const struct wl_interface *WaylandLiriShellV1::interface()
{
    return WaylandLiriShellV1Private::interface();
}

QByteArray WaylandLiriShellV1::interfaceName()
{
    return WaylandLiriShellV1Private::interfaceName();
}

/*
 * WaylandLiriShortcutV1Private
 */

WaylandLiriShortcutV1Private::WaylandLiriShortcutV1Private(WaylandLiriShortcutV1 *self)
    : AuroraObjectPrivate()
    , PrivateServer::zliri_shortcut_v1()
    , q_ptr(self)
{
}

void WaylandLiriShortcutV1Private::zliri_shortcut_v1_destroy_resource(Resource *resource)
{
    Q_UNUSED(resource);
    delete this;
}

void WaylandLiriShortcutV1Private::zliri_shortcut_v1_destroy(Resource *resource)
{
    wl_resource_destroy(resource->handle);
}

/*
 * WaylandLiriShortcutV1
 */

WaylandLiriShortcutV1::WaylandLiriShortcutV1(const QString &sequence,
                                             WaylandCompositor *compositor,
                                             QObject *parent)
    : AuroraObject(*new WaylandLiriShortcutV1Private(this), parent)
{
    Q_D(WaylandLiriShortcutV1);
    d->compositor = compositor;
    d->sequence = sequence;
}

WaylandLiriShortcutV1::~WaylandLiriShortcutV1()
{
}

QString WaylandLiriShortcutV1::sequence() const
{
    Q_D(const WaylandLiriShortcutV1);
    return d->sequence;
}

void WaylandLiriShortcutV1::activate(WaylandSeat *seat)
{
    Q_D(WaylandLiriShortcutV1);

    // TODO: We need a method to retrieve the resource, hopefull will be added to Qt 6.
    // Right now, with Qt 5, we don't support multi-seat anyway
    Q_UNUSED(seat);
    //auto *actualSeat = seat ? seat : d->compositor->defaultSeat();
    d->send_activated(nullptr);
}

/*
 * WaylandLiriOsdV1Private
 */

WaylandLiriOsdV1Private::WaylandLiriOsdV1Private(WaylandLiriOsdV1 *self)
    : WaylandCompositorExtensionPrivate(self)
    , PrivateServer::zliri_osd_v1()
{
}

/*
 * WaylandLiriOsdV1
 */

WaylandLiriOsdV1::WaylandLiriOsdV1()
    : WaylandCompositorExtensionTemplate<WaylandLiriOsdV1>(*new WaylandLiriOsdV1Private(this))
{
}

WaylandLiriOsdV1::WaylandLiriOsdV1(WaylandCompositor *compositor)
    : WaylandCompositorExtensionTemplate<WaylandLiriOsdV1>(compositor, *new WaylandLiriOsdV1Private(this))
{
}

WaylandLiriOsdV1::~WaylandLiriOsdV1()
{
}

void WaylandLiriOsdV1::initialize()
{
    Q_D(WaylandLiriOsdV1);

    WaylandCompositorExtension::initialize();
    WaylandCompositor *compositor = static_cast<WaylandCompositor *>(extensionContainer());
    if (!compositor) {
        qCWarning(gLcLiriShellV1) << "Failed to find WaylandCompositor when initializing WaylandLiriOsd";
        return;
    }
    d->init(compositor->display(), 1);
}

void WaylandLiriOsdV1::showText(const QString &iconName, const QString &text)
{
    Q_D(WaylandLiriOsdV1);

    const auto values = d->resourceMap().values();
    for (auto *resource : qAsConst(values))
        d->send_text(resource->handle, iconName, text);
}

void WaylandLiriOsdV1::showProgress(const QString &iconName, quint32 value)
{
    Q_D(WaylandLiriOsdV1);

    if (value > 100) {
        qCWarning(gLcLiriShellV1) << "Invalid OSD progress value" << value;
        return;
    }

    const auto values = d->resourceMap().values();
    for (auto *resource : qAsConst(values))
        d->send_progress(resource->handle, iconName, value);
}

const struct wl_interface *WaylandLiriOsdV1::interface()
{
    return WaylandLiriOsdV1Private::interface();
}

QByteArray WaylandLiriOsdV1::interfaceName()
{
    return WaylandLiriOsdV1Private::interfaceName();
}
