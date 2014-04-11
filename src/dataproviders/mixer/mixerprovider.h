/****************************************************************************
 * This file is part of Hawaii Shell.
 *
 * Copyright (C) 2012-2014 Pier Luigi Fiorini <pierluigi.fiorini@gmail.com>
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

#ifndef MIXERPROVIDER_H
#define MIXERPROVIDER_H

#include <Hawaii/DataProvider>

class MixerProvider : public Hawaii::DataProvider
{
    Q_OBJECT
    Q_PLUGIN_METADATA(IID "org.hawaii.dataproviders.mixer" FILE "mixer.json")
    Q_INTERFACES(Hawaii::DataProvider)
public:
    MixerProvider(QObject *parent = 0);

public Q_SLOTS:
    void setMute(bool value);
    void setVolume(int value);
};

#endif // MIXERPROVIDER_H
