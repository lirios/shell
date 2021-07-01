// SPDX-FileCopyrightText: 2021 Pier Luigi Fiorini <pierluigi.fiorini@gmail.com>
//
// SPDX-License-Identifier: GPL-3.0-or-later

#include "waylandcompositor.h"

WaylandCompositor::WaylandCompositor()
    : QWaylandQuickCompositor()
{
}

QQmlListProperty<QWaylandOutput> WaylandCompositor::outputsListProperty()
{
    return QQmlListProperty<QWaylandOutput>(this, this,
                                            &WaylandCompositor::countFunction,
                                            &WaylandCompositor::atFunction);
}

int WaylandCompositor::countFunction(QQmlListProperty<QWaylandOutput> *list)
{
    return static_cast<WaylandCompositor *>(list->data)->outputs().size();
}

QWaylandOutput *WaylandCompositor::atFunction(QQmlListProperty<QWaylandOutput> *list, int index)
{
    return static_cast<WaylandCompositor *>(list->data)->outputs().at(index);
}
