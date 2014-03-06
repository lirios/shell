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

#ifndef VOLUMECONTROL_H
#define VOLUMECONTROL_H

#include <QtCore/QObject>

class VolumeControlPrivate;

class VolumeControl : public QObject
{
    Q_OBJECT
    Q_PROPERTY(bool muted READ isMute WRITE setMute NOTIFY muteChanged)
    Q_PROPERTY(int volume READ volume WRITE setVolume NOTIFY volumeChanged)
    Q_DECLARE_PRIVATE(VolumeControl)
public:
    VolumeControl(QObject *parent = 0);
    ~VolumeControl();

    constexpr static const char *name() { return "VolumeControl"; }

    bool isMute() const;
    void setMute(bool value);

    int volume() const;
    void setVolume(int value);

Q_SIGNALS:
    void muteChanged(bool value);
    void volumeChanged(int value);

private:
    Q_PRIVATE_SLOT(d_func(), void _q_upTriggered())
    Q_PRIVATE_SLOT(d_func(), void _q_downTriggered())
    Q_PRIVATE_SLOT(d_func(), void _q_muteTriggered())

    VolumeControlPrivate *const d_ptr;
};

#endif // VOLUMECONTROL_H
