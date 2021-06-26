// SPDX-FileCopyrightText: 2019 Pier Luigi Fiorini <pierluigi.fiorini@gmail.com>
//
// SPDX-License-Identifier: GPL-3.0-or-later

#include "quickoutput.h"

QuickOutput::QuickOutput()
    : QWaylandQuickOutput()
{
}

void QuickOutput::addOutputMode(const QSize &size, int refresh, bool isPreferred, bool isCurrent)
{
    QWaylandOutputMode wlMode(size, refresh);

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

void QuickOutput::setCurrentOutputMode(const QSize &size, int refresh)
{
    QWaylandOutputMode wlMode(size, refresh);
    setCurrentMode(wlMode);
}
