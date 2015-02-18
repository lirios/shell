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

#ifndef WINDOWS_H
#define WINDOWS_H

#include <QtCore/QObject>

#include "qwayland-greenisland.h"

class ApplicationManager;
class Window;

class Windows : public QObject, public QtWayland::greenisland_windows
{
    Q_OBJECT
public:
    Windows(ApplicationManager *parent);
    ~Windows();

    QList<Window *> windows() const;

private:
    ApplicationManager *m_parent;
    QList<Window *> m_windows;

    void windows_window_mapped(greenisland_window *id,
                               uint32_t type, uint32_t state,
                               const QString &title,
                               const QString &appId) Q_DECL_OVERRIDE;
};

#endif // WINDOWS_H
