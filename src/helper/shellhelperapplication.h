/****************************************************************************
 * This file is part of Liri.
 *
 * Copyright (C) 2018 Pier Luigi Fiorini <pierluigi.fiorini@gmail.com>
 *
 * $BEGIN_LICENSE:GPL3+$
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 *
 * $END_LICENSE$
 ***************************************************************************/

#ifndef LIRISHELLHELPERAPPLICATION_H
#define LIRISHELLHELPERAPPLICATION_H

#include <QtCore/QObject>

#include "shellhelperclient.h"

class ShellHelperApplicationPrivate;

class ShellHelperApplication : public QObject
{
    Q_OBJECT
    Q_DECLARE_PRIVATE(ShellHelperApplication)
public:
    explicit ShellHelperApplication(QObject *parent = nullptr);
    ~ShellHelperApplication();

private:
    ShellHelperApplicationPrivate *const d_ptr;

#ifdef HAVE_SYSTEMD
    void setupSystemd();
#endif

private Q_SLOTS:
    void handleCursorChangeRequest(ShellHelperClient::GrabCursor cursor);
#ifdef HAVE_SYSTEMD
    void keepAlive();
#endif
};

#endif // LIRISHELLHELPERAPPLICATION_H
