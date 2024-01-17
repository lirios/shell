// SPDX-FileCopyrightText: 2019-2022 Pier Luigi Fiorini <pierluigi.fiorini@gmail.com>
//
// SPDX-License-Identifier: GPL-3.0-or-later

#include <QtGui/QGuiApplication>
#include <QtGui/QPlatformSurfaceEvent>
#include <QtGui/QWindow>
#include <QtGui/qpa/qplatformnativeinterface.h>

#include "lirishellclientlogging.h"
#include "lirishellv1.h"

// LiriShellV1

LiriShellV1::LiriShellV1()
    : QWaylandClientExtensionTemplate<LiriShellV1>(1)
{
}

LiriShellV1::~LiriShellV1()
{
}

void LiriShellV1::sendReady()
{
    ready();
}

void LiriShellV1::terminateCompositor()
{
    terminate();
}

void LiriShellV1::zliri_shell_v1_logout_requested()
{
    Q_EMIT logoutRequested();
}

void LiriShellV1::zliri_shell_v1_shutdown_requested()
{
    Q_EMIT shutdownRequested();
}

void LiriShellV1::zliri_shell_v1_quit()
{
    Q_EMIT quitRequested();
}

// LiriShortcutV1

LiriShortcutV1::LiriShortcutV1()
    : QWaylandClientExtensionTemplate<LiriShortcutV1>(1)
{
}

LiriShortcutV1::~LiriShortcutV1()
{
    destroy();
}

LiriShellV1 *LiriShortcutV1::shell() const
{
    return m_shell;
}

void LiriShortcutV1::setShell(LiriShellV1 *shell)
{
    if (m_shell == shell)
        return;

    if (isInitialized()) {
        qCWarning(gLcShellClient, "Cannot set LiriShortcutV1::shell after the shortcut is bound");
        return;
    }

    m_shell = shell;
    Q_EMIT shellChanged();
}

QString LiriShortcutV1::sequence() const
{
    return m_sequence;
}

void LiriShortcutV1::setSequence(const QString &sequence)
{
    if (m_sequence == sequence)
        return;

    if (isInitialized()) {
        qCWarning(gLcShellClient, "Cannot set LiriShortcutV1::sequence after the shortcut is bound");
        return;
    }

    m_sequence = sequence;
    Q_EMIT sequenceChanged();
}

void LiriShortcutV1::bindShortcut()
{
    if (isInitialized()) {
        qCWarning(gLcShellClient, "Cannot bind LiriShortcutV1 twice");
        return;
    }

    if (!m_shell) {
        qCWarning(gLcShellClient, "Cannot bind shortcut since LiriShortcutV1::shell is not set");
        return;
    }

    if (m_sequence.isEmpty()) {
        qCWarning(gLcShellClient, "Cannot bind shortcut since LiriShortcutV1::sequence is not set");
        return;
    }

    init(m_shell->bind_shortcut(m_sequence));
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

void LiriShortcutV1::zliri_shortcut_v1_activated(struct ::wl_seat *seat)
{
    // TODO: What to do with wl_seat? It's not a complete type according to QMetaType
    Q_UNUSED(seat);
    Q_EMIT activated();
}

// LiriOsdV1

LiriOsdV1::LiriOsdV1()
    : QWaylandClientExtensionTemplate<LiriOsdV1>(1)
{
}

LiriOsdV1::~LiriOsdV1()
{
}

void LiriOsdV1::zliri_osd_v1_text(const QString &icon_name, const QString &label)
{
    Q_EMIT textRequested(icon_name, label);
}

void LiriOsdV1::zliri_osd_v1_progress(const QString &icon_name, uint32_t value)
{
    Q_EMIT progressRequested(icon_name, value);
}
