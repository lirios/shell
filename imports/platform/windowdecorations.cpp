/****************************************************************************
 * This file is part of Liri Shell.
 *
 * Copyright (C) 2017 Michael Spencer <sonrisesoftware@gmail.com>
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

#include "windowdecorations.h"

#include <QtCore/QTimer>
#include <private/qwaylandabstractdecoration_p.h>
#include <private/qwaylandwindow_p.h>

using namespace QtWaylandClient;

WindowDecorations::WindowDecorations(QObject *parent)
        : QObject(parent), mBackgroundColor("#455a64"), mTextColor("#ffffff"),
          mIconColor("#b4ffffff"), mWindow(nullptr)
{
}

void WindowDecorations::update()
{
    if (!mWindow)
        return;

    QPlatformWindow *platformWindow = mWindow->handle();

    if (platformWindow == nullptr) {
        QTimer::singleShot(10, this, &WindowDecorations::update);
        return;
    }

    QWaylandWindow *waylandWindow = dynamic_cast<QWaylandWindow *>(platformWindow);

    if (waylandWindow == nullptr) {
        return;
    }

    QWaylandAbstractDecoration *decoration = waylandWindow->decoration();

    if (decoration == nullptr) {
        QTimer::singleShot(10, this, &WindowDecorations::update);
        return;
    }

    QWaylandMaterialDecoration *materialDecorations =
            dynamic_cast<QWaylandMaterialDecoration *>(decoration);

    if (materialDecorations == nullptr)
        return;

    materialDecorations->setBackgroundColor(mBackgroundColor);
    materialDecorations->setTextColor(mTextColor);
    materialDecorations->setIconColor(mIconColor);
    materialDecorations->update();
}
