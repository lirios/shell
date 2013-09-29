/****************************************************************************
 * This file is part of Hawaii Shell.
 *
 * Copyright (C) 2013 Pier Luigi Fiorini <pierluigi.fiorini@gmail.com>
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

#ifndef SHELLWINDOW_H
#define SHELLWINDOW_H

#include "desktop-shell.h"
#include "animation.h"

class ShellWindow
{
public:
    ShellWindow(DesktopShell *shell);
    ~ShellWindow();

    DesktopShell *shell() const {
        return m_shell;
    }

    weston_surface *dimmedSurface() const {
        return m_dimmedSurface;
    }

    void connectSignal(wl_signal *signal);

    void createDimmedSurface(weston_output *output);
    void destroyDimmedSurface();

    void animateDialog(weston_surface *surface);

private:
    struct DialogOverlayAnimation;
    struct DialogAnimation;

    DesktopShell *m_shell;
    weston_surface *m_dimmedSurface;
    DialogOverlayAnimation *m_dialogOverlayAnimation;
    DialogAnimation *m_dialogAnimation;
    WlListener m_destroyListener;

    void setDimmedSurfaceAlpha(float alpha);

    void surfaceDestroyed();
};

#endif // SHELLWINDOW_H
