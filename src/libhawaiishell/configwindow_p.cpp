/****************************************************************************
 * This file is part of Hawaii Shell.
 *
 * Copyright (C) 2014 Pier Luigi Fiorini <pierluigi.fiorini@gmail.com>
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

#include <QtQuick/QQuickItem>

#include "configwindow_p.h"

ConfigWindow::ConfigWindow(QWindow *parent)
    : QQuickWindow(parent)
{
    // Transparent color
    setColor(Qt::transparent);

    // This is a frameless window
    setFlags(flags() | Qt::FramelessWindowHint);

    // Create platform window
    create();
}

void ConfigWindow::setContent(QQuickItem *content)
{
    setWidth(content->width());
    setHeight(content->height());
    content->setParentItem(contentItem());
}

#include "moc_configwindow_p.cpp"
