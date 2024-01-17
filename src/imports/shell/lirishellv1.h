// SPDX-FileCopyrightText: 2019-2022 Pier Luigi Fiorini <pierluigi.fiorini@gmail.com>
//
// SPDX-License-Identifier: GPL-3.0-or-later

#pragma once

#include <QObject>
#include <QQmlEngine>
#include <QQmlParserStatus>
#include <QWaylandClientExtension>

#include "qwayland-liri-shell-unstable-v1.h"

QT_FORWARD_DECLARE_CLASS(QWindow)

class LiriShellV1
        : public QWaylandClientExtensionTemplate<LiriShellV1>
        , public QtWayland::zliri_shell_v1
{
    Q_OBJECT
    QML_ELEMENT
public:
    LiriShellV1();
    ~LiriShellV1();

    Q_INVOKABLE void sendReady();
    Q_INVOKABLE void terminateCompositor();

Q_SIGNALS:
    void logoutRequested();
    void shutdownRequested();
    void quitRequested();

private:
    void zliri_shell_v1_logout_requested() override;
    void zliri_shell_v1_shutdown_requested() override;
    void zliri_shell_v1_quit() override;
};

class LiriShortcutV1
        : public QWaylandClientExtensionTemplate<LiriShortcutV1>
        , public QtWayland::zliri_shortcut_v1
        , public QQmlParserStatus
{
    Q_OBJECT
    QML_ELEMENT
    Q_PROPERTY(LiriShellV1 *shell READ shell WRITE setShell NOTIFY shellChanged)
    Q_PROPERTY(QString sequence READ sequence WRITE setSequence NOTIFY sequenceChanged)
    Q_INTERFACES(QQmlParserStatus)
public:
    LiriShortcutV1();
    ~LiriShortcutV1();

    LiriShellV1 *shell() const;
    void setShell(LiriShellV1 *shell);

    QString sequence() const;
    void setSequence(const QString &sequence);

Q_SIGNALS:
    void shellChanged();
    void sequenceChanged();
    void activated();

public Q_SLOTS:
    void bindShortcut();

protected:
    void classBegin() override {}
    void componentComplete() override;

private:
    LiriShellV1 *m_shell = nullptr;
    QString m_sequence;

    void zliri_shortcut_v1_activated(struct ::wl_seat *seat) override;
};

class LiriOsdV1
        : public QWaylandClientExtensionTemplate<LiriOsdV1>
        , public QtWayland::zliri_osd_v1
{
    Q_OBJECT
    QML_ELEMENT
public:
    LiriOsdV1();
    ~LiriOsdV1();

Q_SIGNALS:
    void textRequested(const QString &iconName, const QString &text);
    void progressRequested(const QString &iconName, quint32 value);

private:
    void zliri_osd_v1_text(const QString &icon_name, const QString &label) override;
    void zliri_osd_v1_progress(const QString &icon_name, uint32_t value) override;
};
