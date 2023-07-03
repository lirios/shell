// SPDX-FileCopyrightText: 2019-2022 Pier Luigi Fiorini <pierluigi.fiorini@gmail.com>
//
// SPDX-License-Identifier: GPL-3.0-or-later

#ifndef LIRI_SHELL_COMPOSITOR_WAYLANDLIRISHELLV1_H
#define LIRI_SHELL_COMPOSITOR_WAYLANDLIRISHELLV1_H

#include <LiriAuroraCompositor/WaylandCompositorExtension>

using namespace Aurora::Compositor;

namespace Aurora {

namespace Compositor {

class WaylandOutput;
class WaylandSeat;
class WaylandSurface;

} // namespace Compositor

} // namespace Aurora

class WaylandLiriShellV1Private;
class WaylandLiriShortcutV1Private;
class WaylandLiriOsdV1Private;

class WaylandLiriShortcutV1 : public AuroraObject
{
    Q_OBJECT
    Q_DECLARE_PRIVATE(WaylandLiriShortcutV1)
    Q_PROPERTY(QString sequence READ sequence CONSTANT)
public:
    explicit WaylandLiriShortcutV1(const QString &sequence,
                                   WaylandCompositor *compositor,
                                   QObject *parent = nullptr);
    ~WaylandLiriShortcutV1();

    QString sequence() const;

    Q_INVOKABLE void activate(Aurora::Compositor::WaylandSeat *seat = nullptr);
};

class WaylandLiriShellV1
        : public WaylandCompositorExtensionTemplate<WaylandLiriShellV1>
{
    Q_OBJECT
    Q_DECLARE_PRIVATE(WaylandLiriShellV1)
public:
    WaylandLiriShellV1();
    explicit WaylandLiriShellV1(WaylandCompositor *compositor);
    ~WaylandLiriShellV1();

    void initialize() override;

    Q_INVOKABLE void showPanel(WaylandOutput *output);
    Q_INVOKABLE void requestLogout();
    Q_INVOKABLE void requestShutdown();
    Q_INVOKABLE void sendQuit();

    static const struct wl_interface *interface();
    static QByteArray interfaceName();

Q_SIGNALS:
    void shortcutBound(WaylandLiriShortcutV1 *shortcut);
    void ready(WaylandOutput *output);
    void terminateRequested();
};

class WaylandLiriOsdV1
        : public WaylandCompositorExtensionTemplate<WaylandLiriOsdV1>
{
    Q_OBJECT
    Q_DECLARE_PRIVATE(WaylandLiriOsdV1)
public:
    WaylandLiriOsdV1();
    WaylandLiriOsdV1(WaylandCompositor *compositor);
    ~WaylandLiriOsdV1();

    void initialize() override;

    Q_INVOKABLE void showText(const QString &iconName, const QString &text = QString());
    Q_INVOKABLE void showProgress(const QString &iconName, quint32 value);

    static const struct wl_interface *interface();
    static QByteArray interfaceName();
};

#endif // LIRI_SHELL_COMPOSITOR_WAYLANDLIRISHELLV1_H
