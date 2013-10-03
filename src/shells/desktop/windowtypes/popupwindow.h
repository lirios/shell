/****************************************************************************
 * This file is part of Hawaii Shell.
 *
 * Copyright (C) 2013 Pier Luigi Fiorini <pierluigi.fiorini@gmail.com>
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

#ifndef POPUPWINDOW_H
#define POPUPWINDOW_H

#include <QtQuick/QQuickWindow>

class PopupWindowPrivate;

class PopupWindow : public QQuickWindow
{
    Q_OBJECT
    Q_DECLARE_PRIVATE(PopupWindow)
public:
    PopupWindow(QWindow *parent = 0);
    ~PopupWindow();

protected:
    void showEvent(QShowEvent *event);
    void hideEvent(QHideEvent *event);

private:
    PopupWindowPrivate *const d_ptr;

    void setWindowType();
};

#endif // POPUPWINDOW_H
