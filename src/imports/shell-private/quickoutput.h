// SPDX-FileCopyrightText: 2019 Pier Luigi Fiorini <pierluigi.fiorini@gmail.com>
//
// SPDX-License-Identifier: GPL-3.0-or-later

#ifndef QUICKOUTPUT_H
#define QUICKOUTPUT_H

#include <QWaylandQuickOutput>

class QuickOutput : public QWaylandQuickOutput
{
    Q_OBJECT
public:
    explicit QuickOutput();

    Q_INVOKABLE void addOutputMode(const QSize &size, int refresh, bool isPreferred, bool isCurrent);
    Q_INVOKABLE void setCurrentOutputMode(const QSize &size, int refresh);
};

#endif // QUICKOUTPUT_H
