// SPDX-FileCopyrightText: 2018 Pier Luigi Fiorini <pierluigi.fiorini@gmail.com>
//
// SPDX-License-Identifier: GPL-3.0-or-later

#pragma once

#include <QtCore/QPointer>
#include <QtQuick/QQuickItem>

class KeyEventFilter : public QQuickItem
{
    Q_OBJECT
public:
    KeyEventFilter(QQuickItem *parent = nullptr);

protected:
    bool eventFilter(QObject *, QEvent *);

private:
    QPointer<QQuickWindow> m_window;
};
