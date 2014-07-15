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

#ifndef OVERLAYWINDOW_H
#define OVERLAYWINDOW_H

#include <QtCore/QObject>

class QQuickItem;
class OverlayWindowPrivate;

class OverlayWindow : public QObject
{
    Q_OBJECT
    Q_PROPERTY(QQuickItem *content READ contentItem WRITE setContentItem NOTIFY contentChanged)
    Q_PROPERTY(bool visible READ isVisible WRITE setVisible NOTIFY visibleChanged)
    Q_CLASSINFO("DefaultProperty", "content")
public:
    OverlayWindow(QObject *parent = 0);
    virtual ~OverlayWindow();

    QQuickItem *contentItem() const;
    void setContentItem(QQuickItem *item);

    bool isVisible() const;
    void setVisible(bool value);

public Q_SLOTS:
    void show();
    void hide();

Q_SIGNALS:
    void contentChanged();
    void visibleChanged();

private:
    Q_DECLARE_PRIVATE(OverlayWindow)
    OverlayWindowPrivate *const d_ptr;
};

#endif // OVERLAYWINDOW_H
