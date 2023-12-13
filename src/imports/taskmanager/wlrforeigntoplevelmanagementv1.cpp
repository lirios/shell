// SPDX-FileCopyrightText: 2019-2022 Pier Luigi Fiorini <pierluigi.fiorini@gmail.com>
// SPDX-License-Identifier: GPL-3.0-or-later

#include "utils_p.h"
#include "wlrforeigntoplevelmanagementv1_p.h"
#include "lirishelltaskmanagerlogging.h"

namespace Aurora {

namespace Client {

/*
 * WlrForeignToplevelManagerV1Private
 */

WlrForeignToplevelManagerV1Private::WlrForeignToplevelManagerV1Private(WlrForeignToplevelManagerV1 *self)
    : PrivateClient::zwlr_foreign_toplevel_manager_v1()
    , q_ptr(self)
{
}

void WlrForeignToplevelManagerV1Private::zwlr_foreign_toplevel_manager_v1_toplevel(struct ::zwlr_foreign_toplevel_handle_v1 *toplevel)
{
    Q_Q(WlrForeignToplevelManagerV1);

    auto *handle = new WlrForeignToplevelHandleV1(q);
    handle->d_func()->manager = this;
    handle->d_func()->init(toplevel);
    handles.append(handle);
    emit q->toplevel(handle);
}

void WlrForeignToplevelManagerV1Private::zwlr_foreign_toplevel_manager_v1_finished()
{
}

/*
 * WlrForeignToplevelHandleV1Private
 */

WlrForeignToplevelHandleV1Private::WlrForeignToplevelHandleV1Private(WlrForeignToplevelHandleV1 *self)
    : PrivateClient::zwlr_foreign_toplevel_handle_v1()
    , q_ptr(self)
{
}

WlrForeignToplevelHandleV1Private::~WlrForeignToplevelHandleV1Private()
{
    Q_Q(WlrForeignToplevelHandleV1);
    manager->handles.removeOne(q);
}

void WlrForeignToplevelHandleV1Private::zwlr_foreign_toplevel_handle_v1_title(const QString &title)
{
    pending.title = title;
}

void WlrForeignToplevelHandleV1Private::zwlr_foreign_toplevel_handle_v1_app_id(const QString &appId)
{
    pending.appId = appId;
}

void WlrForeignToplevelHandleV1Private::zwlr_foreign_toplevel_handle_v1_output_enter(wl_output *output)
{
    Q_Q(WlrForeignToplevelHandleV1);

    auto screen = getScreen(output);
    if (!screen) {
        qCWarning(gLcTaskManager, "Cannot find a screen for wl_output");
        return;
    }

    emit q->enteredOutput(screen);
}

void WlrForeignToplevelHandleV1Private::zwlr_foreign_toplevel_handle_v1_output_leave(wl_output *output)
{
    Q_Q(WlrForeignToplevelHandleV1);

    auto screen = getScreen(output);
    if (!screen) {
        qCWarning(gLcTaskManager, "Cannot find a screen for wl_output");
        return;
    }

    emit q->leftOutput(screen);
}

void WlrForeignToplevelHandleV1Private::zwlr_foreign_toplevel_handle_v1_state(wl_array *rawData)
{
    if (rawData) {
        pending.minimized = pending.maximized = pending.fullscreen = pending.activated = false;

        const auto rawStateCount = rawData->size / sizeof(uint32_t);
        uint32_t *s = static_cast<uint32_t *>(rawData->data);
        for (auto i = 0; i < rawStateCount; ++i) {
            uint32_t state = *s++;

            if (state == state_minimized)
                pending.minimized = true;
            if (state == state_maximized)
                pending.maximized = true;
            if (state == state_fullscreen)
                pending.fullscreen = true;
            if (state == state_activated)
                pending.activated = true;
        }
    }
}

void WlrForeignToplevelHandleV1Private::zwlr_foreign_toplevel_handle_v1_done()
{
    Q_Q(WlrForeignToplevelHandleV1);

    // All pending changes are valid now
    if (effective.title != pending.title) {
        effective.title = pending.title;
        emit q->titleChanged(effective.title);
    }
    if (effective.appId != pending.appId) {
        QString oldAppId = effective.appId;
        effective.appId = pending.appId;
        emit q->appIdChanged(oldAppId, effective.appId);
    }
    if (effective.minimized != pending.minimized) {
        effective.minimized = pending.minimized;
        emit q->minimizedChanged(effective.minimized);
    }
    if (effective.maximized != pending.maximized) {
        effective.maximized = pending.maximized;
        emit q->maximizedChanged(effective.maximized);
    }
    if (effective.fullscreen != pending.fullscreen) {
        effective.fullscreen = pending.fullscreen;
        emit q->fullscreenChanged(effective.fullscreen);
    }
    if (effective.activated != pending.activated) {
        effective.activated = pending.activated;
        emit q->activeChanged(effective.activated);
    }
}

void WlrForeignToplevelHandleV1Private::zwlr_foreign_toplevel_handle_v1_closed()
{
    Q_Q(WlrForeignToplevelHandleV1);
    emit q->closed();
}

void WlrForeignToplevelHandleV1Private::zwlr_foreign_toplevel_handle_v1_parent(struct ::zwlr_foreign_toplevel_handle_v1 *parent)
{
    Q_Q(WlrForeignToplevelHandleV1);

    if (!parent) {
        Q_EMIT q->parentChanged(nullptr);
        return;
    }

    for (auto *handle : qAsConst(manager->handles)) {
        auto *handlePriv = WlrForeignToplevelHandleV1Private::get(handle);
        if (handlePriv->object() == parent) {
            Q_EMIT q->parentChanged(handle);
            break;
        }
    }
}

/*
 * WlrForeignToplevelManagerV1
 */

WlrForeignToplevelManagerV1::WlrForeignToplevelManagerV1()
    : QWaylandClientExtensionTemplate<WlrForeignToplevelManagerV1>(3)
    , d_ptr(new WlrForeignToplevelManagerV1Private(this))
{
}

WlrForeignToplevelManagerV1::~WlrForeignToplevelManagerV1()
{
}

void WlrForeignToplevelManagerV1::init(wl_registry *registry, int id, int version)
{
    Q_D(WlrForeignToplevelManagerV1);
    d->init(registry, id, version);
}

const wl_interface *WlrForeignToplevelManagerV1::interface()
{
    return WlrForeignToplevelManagerV1Private::interface();
}

/*
 * WlrForeignToplevelHandleV1
 */

WlrForeignToplevelHandleV1::WlrForeignToplevelHandleV1(QObject *parent)
    : QObject(parent)
    , d_ptr(new WlrForeignToplevelHandleV1Private(this))
{
}

WlrForeignToplevelHandleV1::~WlrForeignToplevelHandleV1()
{
}

QString WlrForeignToplevelHandleV1::title() const
{
    Q_D(const WlrForeignToplevelHandleV1);
    return d->effective.title;
}

QString WlrForeignToplevelHandleV1::appId() const
{
    Q_D(const WlrForeignToplevelHandleV1);
    return d->effective.appId;
}

bool WlrForeignToplevelHandleV1::isMinimized() const
{
    Q_D(const WlrForeignToplevelHandleV1);
    return d->effective.minimized;
}

bool WlrForeignToplevelHandleV1::isMaximized() const
{
    Q_D(const WlrForeignToplevelHandleV1);
    return d->effective.maximized;
}

bool WlrForeignToplevelHandleV1::isFullscreen() const
{
    Q_D(const WlrForeignToplevelHandleV1);
    return d->effective.fullscreen;
}

bool WlrForeignToplevelHandleV1::isActive() const
{
    Q_D(const WlrForeignToplevelHandleV1);
    return d->effective.activated;
}

void WlrForeignToplevelHandleV1::setMinimized()
{
    Q_D(WlrForeignToplevelHandleV1);
    d->set_minimized();
}

void WlrForeignToplevelHandleV1::unsetMinimized()
{
    Q_D(WlrForeignToplevelHandleV1);
    d->unset_minimized();
}

void WlrForeignToplevelHandleV1::setMaximized()
{
    Q_D(WlrForeignToplevelHandleV1);
    d->set_maximized();
}

void WlrForeignToplevelHandleV1::unsetMaximized()
{
    Q_D(WlrForeignToplevelHandleV1);
    d->unset_maximized();
}

void WlrForeignToplevelHandleV1::setFullscreen(QScreen *screen)
{
    Q_D(WlrForeignToplevelHandleV1);

    auto output = getWlOutput(screen);
    if (!output) {
        qCWarning(gLcTaskManager, "Cannot find wl_output for screen %s",
                  qPrintable(screen->name()));
        return;
    }

    d->set_fullscreen(output);
}

void WlrForeignToplevelHandleV1::unsetFullscreen()
{
    Q_D(WlrForeignToplevelHandleV1);
    d->unset_fullscreen();
}

void WlrForeignToplevelHandleV1::activate(struct ::wl_seat *seat)
{
    Q_D(WlrForeignToplevelHandleV1);
    d->activate(seat);
}

void WlrForeignToplevelHandleV1::setRectangle(QWindow *window, const QRect &rect)
{
    Q_D(WlrForeignToplevelHandleV1);
    d->set_rectangle(getWlSurface(window), rect.x(), rect.y(), rect.width(), rect.height());
}

void WlrForeignToplevelHandleV1::close()
{
    Q_D(WlrForeignToplevelHandleV1);
    d->close();
}

} // namespace Client

} // namespace Aurora
