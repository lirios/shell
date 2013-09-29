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

#include "shellwindow.h"

ShellWindow::ShellWindow(DesktopShell *shell)
    : m_shell(shell)
    , m_dimmedSurface(nullptr)
{
}

ShellWindow::~ShellWindow()
{
    if (m_dimmedSurface)
        weston_surface_destroy(m_dimmedSurface);
}

void ShellWindow::connectSignal(wl_signal *signal)
{
    m_destroyListener.listen(signal);
    m_destroyListener.signal->connect(this, &ShellWindow::surfaceDestroyed);
}

void ShellWindow::createDimmedSurface(weston_output *output)
{
    // Can't continue if we already created the surface
    if (m_dimmedSurface)
        return;

    // Create surface
    m_dimmedSurface = m_shell->createBlackSurfaceWithInput(0, 0, 8192, 8192, 0.7);

    // Animation
    m_dimmedAnimation.updateSignal->connect(this, &ShellWindow::setDimmedSurfaceAlpha);
    m_dimmedAnimation.setStart(0.0);
    m_dimmedAnimation.setTarget(0.7);
    m_dimmedAnimation.run(output, 250);
}

void ShellWindow::destroyDimmedSurface()
{
    if (m_dimmedSurface)
        weston_surface_destroy(m_dimmedSurface);
    m_dimmedSurface = nullptr;
}

void ShellWindow::setDimmedSurfaceAlpha(float alpha)
{
    if (!m_dimmedSurface)
        return;

    m_dimmedSurface->alpha = alpha;
    weston_surface_damage(m_dimmedSurface);
}

void ShellWindow::surfaceDestroyed()
{
    delete this;
}
