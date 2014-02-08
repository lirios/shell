/****************************************************************************
 * This file is part of Hawaii Shell.
 *
 * Copyright (C) 2012-2014 Pier Luigi Fiorini <pierluigi.fiorini@gmail.com>
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

#ifndef POPUPQUICKWINDOW_P_H
#define POPUPQUICKWINDOW_P_H

#include <QtQuick/QQuickWindow>

#include "qwayland-hawaii.h"

class PopupWindow;

class PopupSurface : public QtWayland::wl_hawaii_popup_surface
{
public:
    PopupSurface(PopupWindow *popup);
    virtual ~PopupSurface();

    QWindow *parentWindow() const;
    QPointF mapToParent(const QPointF &pos);

protected:
    void hawaii_popup_surface_popup_done();

private:
    PopupWindow *m_popup;
};

class PopupQuickWindow : public QQuickWindow
{
    Q_OBJECT
public:
    PopupQuickWindow(PopupWindow *parent);
    virtual ~PopupQuickWindow();

public Q_SLOTS:
    void setWindowType();

private:
    PopupSurface *m_popupSurface;
};

#endif // POPUPQUICKWINDOW_P_H
