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

#include "shellwindow.h"

struct ShellWindow::DialogOverlayAnimation
{
    Animation animation;
    weston_view *view;

    void animate(float value)
    {
        view->alpha = value;
        weston_surface_damage(view->surface);
    }
};

struct ShellWindow::DialogAnimation
{
    Animation animation;
    weston_view *view;
    weston_transform transform;
    float targetX;
    float targetY;

    void animate(float value)
    {
        weston_matrix *matrix = &transform.matrix;
        weston_matrix_init(matrix);
        weston_matrix_scale(matrix, value, value, 1.0);
        weston_matrix_translate(matrix, targetX * (1.0 - value),
                                targetY * (1.0 - value), 0);
        weston_view_geometry_dirty(view);
        weston_view_update_transform(view);
        weston_surface_damage(view->surface);
    }

    void done()
    {
        if (!wl_list_empty(&transform.link)) {
            wl_list_remove(&transform.link);
            wl_list_init(&transform.link);

            weston_view_geometry_dirty(view);
            weston_view_update_transform(view);
            weston_surface_damage(view->surface);
        }
    }
};

ShellWindow::ShellWindow(DesktopShell *shell)
    : m_shell(shell)
    , m_dimmedSurface(nullptr)
    , m_dialogOverlayAnimation(nullptr)
    , m_dialogAnimation(nullptr)
{
}

ShellWindow::~ShellWindow()
{
    if (m_dimmedSurface)
        weston_surface_destroy(m_dimmedSurface->surface);
    delete m_dialogOverlayAnimation;
    delete m_dialogAnimation;
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
    m_dialogOverlayAnimation = new DialogOverlayAnimation;
    m_dialogOverlayAnimation->view = m_dimmedSurface;

    m_dialogOverlayAnimation->animation.updateSignal->connect(
                m_dialogOverlayAnimation, &DialogOverlayAnimation::animate);

    m_dialogOverlayAnimation->animation.setStart(0.0);
    m_dialogOverlayAnimation->animation.setTarget(0.7);
    m_dialogOverlayAnimation->animation.run(output, 250);
}

void ShellWindow::destroyDimmedSurface()
{
    if (m_dimmedSurface) {
        weston_surface_destroy(m_dimmedSurface->surface);
        weston_view_destroy(m_dimmedSurface);
    }
    m_dimmedSurface = nullptr;
}

void ShellWindow::animateDialog(weston_view *view)
{
    if (m_dialogAnimation)
        return;

    m_dialogAnimation = new DialogAnimation;
    m_dialogAnimation->view = view;
    m_dialogAnimation->targetX = view->surface->width / 2.0;
    m_dialogAnimation->targetY = view->surface->height / 2.0;
    wl_list_init(&m_dialogAnimation->transform.link);

    m_dialogAnimation->animation.updateSignal->connect(
                m_dialogAnimation, &DialogAnimation::animate);
    m_dialogAnimation->animation.doneSignal->connect(
                m_dialogAnimation, &DialogAnimation::done);

    weston_matrix *matrix = &m_dialogAnimation->transform.matrix;
    weston_matrix_init(matrix);
    weston_matrix_scale(matrix, 0.01, 0.01, 1.0);
    wl_list_insert(&view->geometry.transformation_list, &m_dialogAnimation->transform.link);

    m_dialogAnimation->animation.setStart(0.01);
    m_dialogAnimation->animation.setTarget(1.0);
    m_dialogAnimation->animation.run(view->output, 200, Animation::Flags::SendDone);
}

void ShellWindow::surfaceDestroyed(void *)
{
    delete this;
}
