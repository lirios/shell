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

#ifndef POPUPWINDOW_H
#define POPUPWINDOW_H

#include <QtCore/QObject>

class QQuickItem;
class PopupWindowPrivate;

class PopupWindow : public QObject
{
    Q_OBJECT
    Q_PROPERTY(QQuickItem *content READ contentItem WRITE setContentItem NOTIFY contentChanged)
    Q_PROPERTY(qreal x READ x WRITE setX NOTIFY xChanged)
    Q_PROPERTY(qreal y READ y WRITE setY NOTIFY yChanged)
    Q_PROPERTY(qreal width READ width WRITE setWidth NOTIFY widthChanged)
    Q_PROPERTY(qreal height READ height WRITE setHeight NOTIFY heightChanged)
    Q_PROPERTY(bool visible READ isVisible WRITE setVisible NOTIFY visibleChanged)
    Q_CLASSINFO("DefaultProperty", "content")
public:
    PopupWindow(QObject *parent = 0);
    virtual ~PopupWindow();

    QQuickItem *contentItem() const;
    void setContentItem(QQuickItem *item);

    qreal x() const;
    void setX(qreal x);

    qreal y() const;
    void setY(qreal y);

    qreal width() const;
    void setWidth(qreal width);

    qreal height() const;
    void setHeight(qreal height);

    bool isVisible() const;
    void setVisible(bool value);

public Q_SLOTS:
    void show();
    void hide();

Q_SIGNALS:
    void contentChanged();
    void xChanged();
    void yChanged();
    void widthChanged();
    void heightChanged();
    void visibleChanged();

private:
    Q_DECLARE_PRIVATE(PopupWindow)
    PopupWindowPrivate *const d_ptr;

private Q_SLOTS:
    void close();
};

#endif // POPUPWINDOW_H
