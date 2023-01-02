// SPDX-FileCopyrightText: 2019 Pier Luigi Fiorini <pierluigi.fiorini@gmail.com>
//
// SPDX-License-Identifier: GPL-3.0-or-later

#include "lirishellcompositorlogging.h"
#include "quickoutput.h"

QuickOutput::QuickOutput()
    : Aurora::Compositor::WaylandQuickOutput()
{
}

ScreenItem *QuickOutput::screen() const
{
    return m_screen;
}

void QuickOutput::setScreen(ScreenItem *screen)
{
    if (m_screen == screen)
        return;

    if (m_screen) {
        if (m_initialized) {
            qCWarning(lcShellCompositor, "Cannot change WaylandOutput::screen after the component is complete");
            return;
        } else {
            disconnect(this, SLOT(handleCurrentModeChanged(QSize,int)));
        }
    }

    m_screen = screen;
    connect(m_screen, SIGNAL(currentModeChanged(QSize,int)),
            this, SLOT(handleCurrentModeChanged(QSize,int)));
    addModes();
    Q_EMIT screenChanged(screen);
}

void QuickOutput::componentComplete()
{
    m_initialized = true;
    WaylandQuickOutput::componentComplete();
}

void QuickOutput::addModes()
{
    auto modes = m_screen->modes();
    for (auto *mode : qAsConst(modes))
        addScreenMode(mode);
}

void QuickOutput::addScreenMode(ScreenMode *mode)
{
    auto isPreferred = m_screen->preferredMode()->resolution() == mode->resolution() &&
            m_screen->preferredMode()->refreshRate() == mode->refreshRate();
    auto isCurrent = m_screen->currentMode()->resolution() == mode->resolution() &&
            m_screen->currentMode()->refreshRate() == mode->refreshRate();
    WaylandOutputMode wlMode(mode->resolution(), mode->refreshRate());

    bool alreadyHasMode = false;
    const auto modesList = modes();
    for (const auto &mode : modesList) {
        if (mode == wlMode) {
            alreadyHasMode = true;
            break;
        }
    }

    if (!alreadyHasMode)
        addMode(wlMode, isPreferred);

    if (isCurrent)
        setCurrentMode(wlMode);
}

void QuickOutput::handleCurrentModeChanged(const QSize &resolution, int refreshRate)
{
    setCurrentMode(WaylandOutputMode(resolution, refreshRate));
}
