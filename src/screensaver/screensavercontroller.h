/****************************************************************************
 * This file is part of Hawaii Shell.
 *
 * Copyright (C) 2013-2014 Pier Luigi Fiorini <pierluigi.fiorini@gmail.com>
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

#ifndef SCREENSAVERCONTROLLER_H
#define SCREENSAVERCONTROLLER_H

#include <QtCore/QObject>
#include <QtGui/QScreen>

#include "qwayland-screensaver.h"

class ScreenSaverView;

class ScreenSaverController : public QObject
{
    Q_OBJECT
public:
    explicit ScreenSaverController(QObject *parent = 0);
    ~ScreenSaverController();

    void initialize();

private Q_SLOTS:
    void run();

    void screenAdded(QScreen *screen);
    void screenRemoved(QObject *object = 0);

private:
    QtWayland::wl_screensaver *m_screensaver;
    QList<ScreenSaverView *> m_windows;

    void setupWindow(ScreenSaverView *window);

    static void handleGlobal(void *data, wl_registry *registry,
                             uint32_t id, const char *interface,
                             uint32_t version);
    static void handleGlobalRemove(void *data,
                                   wl_registry *registry,
                                   uint32_t name);

    static const struct wl_registry_listener registryListener;
};

#endif // SCREENSAVERCONTROLLER_H
