/****************************************************************************
 * This file is part of Hawaii Shell.
 *
 * Copyright (C) 2014 Pier Luigi Fiorini <pierluigi.fiorini@gmail.com>
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

#ifndef ELEMENTINFO_H
#define ELEMENTINFO_H

#include <QtCore/QDir>

#include <HawaiiShell/Element>
#include <HawaiiShell/PluginMetadata>

class ElementInfo
{
public:
    explicit ElementInfo(const QString &fileName);

    Hawaii::Shell::PluginMetadata metadata() const;

    QDir directory() const;

    Hawaii::Shell::Element *createElement();

private:
    Hawaii::Shell::PluginMetadata m_metadata;
    QDir m_dir;
};

#endif // ELEMENTINFO_H
