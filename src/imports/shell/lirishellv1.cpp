// SPDX-FileCopyrightText: 2019-2022 Pier Luigi Fiorini <pierluigi.fiorini@gmail.com>
//
// SPDX-License-Identifier: GPL-3.0-or-later

#include <QtGui/QGuiApplication>
#include <QtGui/QPlatformSurfaceEvent>
#include <QtGui/QWindow>
#include <QtGui/qpa/qplatformnativeinterface.h>

#include "lirishellv1_p.h"
#include "lirishellclientlogging.h"

namespace Aurora {

namespace Client {

// LiriShellV1Private

LiriShellV1Private::LiriShellV1Private(LiriShellV1 *qq)
    : PrivateClient::zliri_shell_v1()
    , q_ptr(qq)
{
}

void LiriShellV1Private::zliri_shell_v1_logout_requested()
{
    Q_Q(LiriShellV1);
    Q_EMIT q->logoutRequested();
}

void LiriShellV1Private::zliri_shell_v1_shutdown_requested()
{
    Q_Q(LiriShellV1);
    Q_EMIT q->shutdownRequested();
}

void LiriShellV1Private::zliri_shell_v1_quit()
{
    Q_Q(LiriShellV1);
    emit q->quitRequested();
}

// LiriShellV1

LiriShellV1::LiriShellV1()
    : QWaylandClientExtensionTemplate(1)
    , d_ptr(new LiriShellV1Private(this))
{
}

LiriShellV1::~LiriShellV1()
{
}

void LiriShellV1::init(wl_registry *registry, int id, int version)
{
    Q_D(LiriShellV1);
    d->init(registry, id, version);
}

void LiriShellV1::sendReady()
{
    Q_D(LiriShellV1);
    d->ready();
}

void LiriShellV1::terminateCompositor()
{
    Q_D(LiriShellV1);
    d->terminate();
}

const wl_interface *LiriShellV1::interface()
{
    return PrivateClient::zliri_shell_v1::interface();
}

// LiriShortcutV1Private

LiriShortcutV1Private::LiriShortcutV1Private(LiriShortcutV1 *self)
    : PrivateClient::zliri_shortcut_v1()
    , q_ptr(self)
{
}

LiriShortcutV1Private::~LiriShortcutV1Private()
{
    destroy();
}

void LiriShortcutV1Private::zliri_shortcut_v1_activated(struct ::wl_seat *seat)
{
    Q_Q(LiriShortcutV1);
    Q_EMIT q->activated(seat);
}

// LiriShortcutV1

LiriShortcutV1::LiriShortcutV1()
    : QWaylandClientExtension(1)
    , d_ptr(new LiriShortcutV1Private(this))
{
}

LiriShortcutV1::~LiriShortcutV1()
{
}

LiriShellV1 *LiriShortcutV1::shell() const
{
    Q_D(const LiriShortcutV1);
    return d->shell;
}

void LiriShortcutV1::setShell(LiriShellV1 *shell)
{
    Q_D(LiriShortcutV1);

    if (d->shell == shell)
        return;

    if (d->isInitialized()) {
        qCWarning(gLcShellClient, "Cannot set LiriShortcutV1::shell after the shortcut is bound");
        return;
    }

    d->shell = shell;
    Q_EMIT shellChanged();
}

QString LiriShortcutV1::sequence() const
{
    Q_D(const LiriShortcutV1);
    return d->sequence;
}

void LiriShortcutV1::setSequence(const QString &sequence)
{
    Q_D(LiriShortcutV1);

    if (d->sequence == sequence)
        return;

    if (d->isInitialized()) {
        qCWarning(gLcShellClient, "Cannot set LiriShortcutV1::sequence after the shortcut is bound");
        return;
    }

    d->sequence = sequence;
    Q_EMIT sequenceChanged();
}

void LiriShortcutV1::bind(struct ::wl_registry *registry, int id, int ver)
{
    Q_UNUSED(registry);
    Q_UNUSED(id);
    Q_UNUSED(ver);
}

const wl_interface *LiriShortcutV1::interface()
{
    return LiriShortcutV1Private::interface();
}

void LiriShortcutV1::bindShortcut()
{
    Q_D(LiriShortcutV1);

    if (d->isInitialized()) {
        qCWarning(gLcShellClient, "Cannot bind LiriShortcutV1 twice");
        return;
    }

    if (!d->shell) {
        qCWarning(gLcShellClient, "Cannot bind shortcut since LiriShortcutV1::shell is not set");
        return;
    }

    if (d->sequence.isEmpty()) {
        qCWarning(gLcShellClient, "Cannot bind shortcut since LiriShortcutV1::sequence is not set");
        return;
    }

    d->init(LiriShellV1Private::get(d->shell)->bind_shortcut(d->sequence));
}

void LiriShortcutV1::componentComplete()
{
    // Find the shell from the parent, if not specified
    if (!shell()) {
        for (auto *p = parent(); p != nullptr; p = p->parent()) {
            if (auto *s = qobject_cast<LiriShellV1 *>(p)) {
                setShell(s);
                break;
            } else if (auto *s = p->findChild<LiriShellV1 *>()) {
                setShell(s);
                break;
            }
        }
    }

    QMetaObject::invokeMethod(this, "bindShortcut", Qt::QueuedConnection);
}

// LiriOsdV1Private

LiriOsdV1Private::LiriOsdV1Private(LiriOsdV1 *self)
    : PrivateClient::zliri_osd_v1()
    , q_ptr(self)
{
}

void LiriOsdV1Private::zliri_osd_v1_text(const QString &icon_name, const QString &label)
{
    Q_Q(LiriOsdV1);
    emit q->textRequested(icon_name, label);
}

void LiriOsdV1Private::zliri_osd_v1_progress(const QString &icon_name, uint32_t value)
{
    Q_Q(LiriOsdV1);
    emit q->progressRequested(icon_name, value);
}

// LiriOsdV1

LiriOsdV1::LiriOsdV1()
    : QWaylandClientExtensionTemplate(1)
    , d_ptr(new LiriOsdV1Private(this))
{
}

LiriOsdV1::~LiriOsdV1()
{
}

void LiriOsdV1::init(struct ::wl_registry *registry, int id, int version)
{
    Q_D(LiriOsdV1);
    d->init(registry, id, version);
}

const wl_interface *LiriOsdV1::interface()
{
    return LiriOsdV1Private::interface();
}

} // namespace Client

} // namespace Aurora
