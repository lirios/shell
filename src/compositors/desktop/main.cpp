/****************************************************************************
 * This file is part of Hawaii Shell.
 *
 * Copyright (C) 2012-2013 Pier Luigi Fiorini <pierluigi.fiorini@gmail.com>
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

#include <GreenIsland/VCompositorPlugin>

#include "compositor.h"

class GREENISLAND_EXPORT DesktopCompositorPlugin : public VCompositorPlugin
{
    Q_OBJECT
    Q_PLUGIN_METADATA(IID "org.hawaii.GreenIsland.VCompositorFactoryInterface" FILE "desktop.json")
public:
    QStringList keys() const {
        return QStringList() << "desktop";
    }

    VCompositor *create(const QString &key, const QRect &geometry) {
        if (key.toLower() == "desktop")
            return new DesktopCompositor(geometry);

        return 0;
    }
};

#include "main.moc"
