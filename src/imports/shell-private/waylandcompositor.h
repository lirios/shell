// SPDX-FileCopyrightText: 2021 Pier Luigi Fiorini <pierluigi.fiorini@gmail.com>
//
// SPDX-License-Identifier: GPL-3.0-or-later

#ifndef WAYLANDCOMPOSITOR_H
#define WAYLANDCOMPOSITOR_H

#include <QWaylandQuickCompositor>
#include <QQmlListProperty>

class WaylandCompositor : public QWaylandQuickCompositor
{
    Q_OBJECT
    Q_PROPERTY(QQmlListProperty<QWaylandOutput> outputs READ outputsListProperty DESIGNABLE false)
public:
    WaylandCompositor();

    QQmlListProperty<QWaylandOutput> outputsListProperty();

    static int countFunction(QQmlListProperty<QWaylandOutput> *list);
    static QWaylandOutput *atFunction(QQmlListProperty<QWaylandOutput> *list, int index);
};

#endif // WAYLANDCOMPOSITOR_H
