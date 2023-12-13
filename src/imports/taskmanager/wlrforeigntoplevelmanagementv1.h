// SPDX-FileCopyrightText: 2019-2022 Pier Luigi Fiorini <pierluigi.fiorini@gmail.com>
// SPDX-License-Identifier: GPL-3.0-or-later

#pragma once

#include <QWaylandClientExtension>

#include <wayland-client.h>

QT_FORWARD_DECLARE_CLASS(QWindow)
QT_FORWARD_DECLARE_CLASS(QScreen)

namespace Aurora {

namespace Client {

class WlrForeignToplevelManagerV1Private;
class WlrForeignToplevelHandleV1;
class WlrForeignToplevelHandleV1Private;

class WlrForeignToplevelManagerV1
        : public QWaylandClientExtensionTemplate<WlrForeignToplevelManagerV1>
{
    Q_OBJECT
    Q_DECLARE_PRIVATE(WlrForeignToplevelManagerV1)
public:
    explicit WlrForeignToplevelManagerV1();
    ~WlrForeignToplevelManagerV1();

    void init(struct ::wl_registry *registry, int id, int version);

    static const wl_interface *interface();

Q_SIGNALS:
    void toplevel(Aurora::Client::WlrForeignToplevelHandleV1 *handle);

private:
    QScopedPointer<WlrForeignToplevelManagerV1Private> const d_ptr;
};

class WlrForeignToplevelHandleV1 : public QObject
{
    Q_OBJECT
    Q_DECLARE_PRIVATE(WlrForeignToplevelHandleV1)
    Q_PROPERTY(QString title READ title NOTIFY titleChanged)
    Q_PROPERTY(QString appId READ appId NOTIFY appIdChanged)
    Q_PROPERTY(bool minimized READ isMinimized NOTIFY minimizedChanged)
    Q_PROPERTY(bool maximized READ isMaximized NOTIFY maximizedChanged)
    Q_PROPERTY(bool fullscreen READ isFullscreen NOTIFY fullscreenChanged)
    Q_PROPERTY(bool active READ isActive NOTIFY activeChanged)
public:
    explicit WlrForeignToplevelHandleV1(QObject *parent = nullptr);
    ~WlrForeignToplevelHandleV1();

    QString title() const;
    QString appId() const;

    bool isMinimized() const;
    bool isMaximized() const;
    bool isFullscreen() const;
    bool isActive() const;

    Q_INVOKABLE void setMinimized();
    Q_INVOKABLE void unsetMinimized();

    Q_INVOKABLE void setMaximized();
    Q_INVOKABLE void unsetMaximized();

    Q_INVOKABLE void setFullscreen(QScreen *screen);
    Q_INVOKABLE void unsetFullscreen();

    Q_INVOKABLE void activate(struct ::wl_seat *seat);

    Q_INVOKABLE void setRectangle(QWindow *window, const QRect &rect);

    Q_INVOKABLE void close();

Q_SIGNALS:
    void titleChanged(const QString &title);
    void appIdChanged(const QString &oldAppId, const QString &newAppId);
    void minimizedChanged(bool minimized);
    void maximizedChanged(bool maximized);
    void fullscreenChanged(bool fullscreen);
    void activeChanged(bool activated);
    void parentChanged(Aurora::Client::WlrForeignToplevelHandleV1 *parentHandle);
    void enteredOutput(QScreen *screen);
    void leftOutput(QScreen *screen);
    void closed();

private:
    QScopedPointer<WlrForeignToplevelHandleV1Private> const d_ptr;

    friend class WlrForeignToplevelManagerV1Private;
};

} // namespace Client

} // namespace Aurora
