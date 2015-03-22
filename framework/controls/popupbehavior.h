/****************************************************************************
 * This file is part of Hawaii.
 *
 * Copyright (C) 2015 Pier Luigi Fiorini
 *
 * Author(s):
 *    Pier Luigi Fiorini <pierluigi.fiorini@gmail.com>
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

#ifndef POPUPBEHAVIOR_H
#define POPUPBEHAVIOR_H

#include <QtCore/QPointer>
#include <QtQuick/QQuickItem>

class PopupBehavior : public QQuickItem
{
    Q_OBJECT
public:
    PopupBehavior(QQuickItem *parent = 0);

Q_SIGNALS:
    void clickedInside();
    void dismissed();

protected:
    bool eventFilter(QObject *object, QEvent *event) Q_DECL_OVERRIDE;
    void itemChange(ItemChange change, const ItemChangeData &data) Q_DECL_OVERRIDE;
    void componentComplete() Q_DECL_OVERRIDE;

private:
    bool m_ready;
    QPointer<QObject> m_filterHost;

    void updateEventFilter(bool enable);
    bool processEvent(QEvent *event);

private Q_SLOTS:
    void update();
};

#endif // POPUPBEHAVIOR_H
