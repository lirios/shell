/****************************************************************************
 * This file is part of Hawaii.
 *
 * Copyright (C) 2015 Pier Luigi Fiorini
 *
 * Author(s):
 *    Pier Luigi Fiorini <pierluigi.fiorini@gmail.com>
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

#ifndef WINDOW_H
#define WINDOW_H

#include <QtCore/QObject>

#include "qwayland-greenisland.h"

class Windows;

class Window : public QObject, public QtWayland::greenisland_window
{
    Q_OBJECT
    Q_PROPERTY(Type type READ type CONSTANT)
    Q_PROPERTY(QString title READ title NOTIFY titleChanged)
    Q_PROPERTY(QString appId READ appId NOTIFY appIdChanged)
    Q_PROPERTY(quint32 workspace READ workspace NOTIFY workspaceChanged)
    Q_PROPERTY(bool active READ isActive NOTIFY stateChanged)
    Q_PROPERTY(bool minimized READ isMinimized NOTIFY stateChanged)
    Q_PROPERTY(bool maximized READ isMaximized NOTIFY stateChanged)
    Q_PROPERTY(bool fullScreen READ isFullScreen NOTIFY stateChanged)
    Q_ENUMS(Type)
public:
    enum Type {
        TopLevel,
        Popup,
        Transient
    };

    Window(struct ::greenisland_window *object, Type type, QObject *parent = 0);
    ~Window();

    Type type() const;
    QString title() const;
    QString appId() const;

    quint32 workspace() const;

    bool isActive() const;
    bool isMinimized() const;
    bool isMaximized() const;
    bool isFullScreen() const;

Q_SIGNALS:
    void titleChanged();
    void appIdChanged();
    void stateChanged();
    void workspaceChanged();
    void unmapped();

private:
    Type m_type;
    quint32 m_state;
    QString m_title;
    QString m_appId;
    quint32 m_workspace;

    void window_title_changed(const QString &title) Q_DECL_OVERRIDE;
    void window_app_id_changed(const QString &appId) Q_DECL_OVERRIDE;
    void window_state_changed(uint32_t flags) Q_DECL_OVERRIDE;
    void window_workspace_changed(uint32_t number) Q_DECL_OVERRIDE;
    void window_unmapped() Q_DECL_OVERRIDE;

    friend class Windows;
};

#endif // WINDOW_H
