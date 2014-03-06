/****************************************************************************
 * This file is part of Hawaii Shell.
 *
 * Copyright (C) 2013-2014 Pier Luigi Fiorini <pierluigi.fiorini@gmail.com>
 * Copyright (C) 2013 Marco Martin <mart@kde.org>
 *
 * Author(s):
 *    Pier Luigi Fiorini
 *
 * $BEGIN_LICENSE:GPL2+$
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 *
 * $END_LICENSE$
 ***************************************************************************/

#ifndef PANELCONFIGVIEW_H
#define PANELCONFIGVIEW_H

#include "configview.h"
#include "panelview.h"

class PanelView;

class PanelConfigView : public ConfigView
{
    Q_OBJECT
public:
    explicit PanelConfigView(PanelView *parent);

protected:
    void focusOutEvent(QFocusEvent *event);

private:
    PanelView *m_panelView;

    void setSurfaceRole();
    void setSurfacePosition();

private Q_SLOTS:
    void setupGeometry();
};

#endif // PANELCONFIGVIEW_H
