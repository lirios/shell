// SPDX-FileCopyrightText: 2020-2022 Pier Luigi Fiorini <pierluigi.fiorini@gmail.com>
//
// SPDX-License-Identifier: GPL-3.0-or-later

#include <LiriAuroraCompositor/WaylandCompositor>

#include "waylandlirimodalv1_p.h"

Q_LOGGING_CATEGORY(gLcWaylandLiriModalV1, "liri.shell.lirimodalv1")

/*
 * WaylandLiriModalV1
 */

void WaylandLiriModalV1::zliri_modal_v1_destroy_resource(Resource *resource)
{
    Q_UNUSED(resource);
    manager->removeGrab(this);
    delete this;
}

void WaylandLiriModalV1::zliri_modal_v1_destroy(Resource *resource)
{
    wl_resource_destroy(resource->handle);
}

/*
 * WaylandLiriModalManagerV1Private
 */

WaylandLiriModalManagerV1Private::WaylandLiriModalManagerV1Private(WaylandLiriModalManagerV1 *self)
    : PrivateServer::zliri_modal_manager_v1()
    , q_ptr(self)
{
}

void WaylandLiriModalManagerV1Private::removeGrab(WaylandLiriModalV1 *grab)
{
    Q_Q(WaylandLiriModalManagerV1);

    if (grabs.removeOne(grab))
        Q_EMIT q->grabbedChanged();
}

void WaylandLiriModalManagerV1Private::zliri_modal_manager_v1_grab(Resource *resource, uint32_t id,
                                                                   struct ::wl_resource *seatResource)
{
    Q_UNUSED(seatResource);
    Q_Q(WaylandLiriModalManagerV1);

    auto *modal = new WaylandLiriModalV1();
    modal->manager = this;
    modal->init(resource->client(), id, WaylandLiriModalV1::interfaceVersion());

    grabs.append(modal);
    Q_EMIT q->grabbedChanged();
}

/*
 * WaylandLiriModalManagerV1
 */

WaylandLiriModalManagerV1::WaylandLiriModalManagerV1()
    : WaylandCompositorExtensionTemplate<WaylandLiriModalManagerV1>()
    , d_ptr(new WaylandLiriModalManagerV1Private(this))
{
}

WaylandLiriModalManagerV1::WaylandLiriModalManagerV1(WaylandCompositor *compositor)
    : WaylandCompositorExtensionTemplate<WaylandLiriModalManagerV1>(compositor)
    , d_ptr(new WaylandLiriModalManagerV1Private(this))
{
}

WaylandLiriModalManagerV1::~WaylandLiriModalManagerV1()
{
}

void WaylandLiriModalManagerV1::initialize()
{
    Q_D(WaylandLiriModalManagerV1);

    WaylandCompositorExtensionTemplate::initialize();
    WaylandCompositor *compositor = static_cast<WaylandCompositor *>(extensionContainer());
    if (!compositor) {
        qCWarning(gLcWaylandLiriModalV1) << "Failed to find WaylandCompositor when initializing WaylandLiriModal";
        return;
    }
    d->init(compositor->display(), WaylandLiriModalManagerV1Private::interfaceVersion());
}

bool WaylandLiriModalManagerV1::isGrabbed() const
{
    Q_D(const WaylandLiriModalManagerV1);
    return d->grabs.size() > 0;
}

void WaylandLiriModalManagerV1::sendDone()
{
    Q_D(WaylandLiriModalManagerV1);

    bool changed = false;

    auto it = d->grabs.begin();
    while (it != d->grabs.end()) {
        (*it)->send_done();
        it = d->grabs.erase(it);
        changed = true;
    }

    if (changed)
        Q_EMIT grabbedChanged();
}

const wl_interface *WaylandLiriModalManagerV1::interface()
{
    return WaylandLiriModalManagerV1Private::interface();
}

QByteArray WaylandLiriModalManagerV1::interfaceName()
{
    return WaylandLiriModalManagerV1Private::interfaceName();
}
