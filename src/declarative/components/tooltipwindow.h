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

#ifndef TOOLTIPWINDOW_H
#define TOOLTIPWINDOW_H

#include <QtQuick/QQuickItem>

class TooltipWindow : public QQuickItem
{
    Q_OBJECT
    Q_PROPERTY(QQuickItem *content READ content WRITE setContent)
    Q_CLASSINFO("DefaultProperty", "content")
public:
    TooltipWindow(QQuickItem *parent = 0);
    virtual ~TooltipWindow();

    QQuickItem *content() const;
    void setContent(QQuickItem *item);

public Q_SLOTS:
    void show();
    void hide();

protected:
    bool eventFilter(QObject *object, QEvent *event);

private:
    QTimer *m_showTimer;
    QTimer *m_hideTimer;
    int m_showCount;
    QQuickItem *m_content;
    QQuickWindow *m_window;

private Q_SLOTS:
    void showWindow();
    void hideWindow();
};

#endif // TOOLTIPWINDOW_H
