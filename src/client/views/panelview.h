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

#ifndef PANELVIEW_H
#define PANELVIEW_H

#include <QtQuick/QQuickView>

#include <HawaiiShell/HawaiiShell>
#include <HawaiiShell/QuickView>

class QQmlEngine;
class QScreen;

class PanelView : public Hawaii::Shell::QuickView
{
    Q_OBJECT
    Q_PROPERTY(Hawaii::Shell::Types::Location location READ location WRITE setLocation NOTIFY locationChanged)
    Q_PROPERTY(Qt::Alignment alignment READ alignment WRITE setAlignment NOTIFY alignmentChanged)
public:
    explicit PanelView(QQmlEngine *engine, QScreen *screen);

    Hawaii::Shell::Types::Location location() const;
    void setLocation(Hawaii::Shell::Types::Location location);

    Qt::Alignment alignment() const;
    void setAlignment(Qt::Alignment alignment);

Q_SIGNALS:
    void locationChanged();
    void alignmentChanged();

private:
    Hawaii::Shell::Types::Location m_location;
    Qt::Alignment m_alignment;

    void setWindowType();
};

#endif // PANELVIEW_H
