/****************************************************************************
 * This file is part of Hawaii Shell.
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

#ifndef APPLICATIONS_H
#define APPLICATIONS_H

#include "qwayland-greenisland.h"

class ApplicationManager;

class Applications : public QtWayland::greenisland_applications
{
public:
    Applications(ApplicationManager *parent);
    ~Applications();

    void applications_registered(const QString &appId) Q_DECL_OVERRIDE;
    void applications_unregistered(const QString &appId) Q_DECL_OVERRIDE;
    void applications_focused(const QString &appId) Q_DECL_OVERRIDE;
    void applications_unfocused(const QString &appId) Q_DECL_OVERRIDE;

private:
    ApplicationManager *m_parent;
};

#endif // APPLICATIONS_H
