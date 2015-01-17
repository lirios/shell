/****************************************************************************
 * This file is part of Hawaii.
 *
 * Copyright (C) 2015 Pier Luigi Fiorini <pierluigi.fiorini@gmail.com>
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

#ifndef MIXER_H
#define MIXER_H

#include <QtCore/QObject>
#include <QtCore/QLoggingCategory>

Q_DECLARE_LOGGING_CATEGORY(MIXER)

class MixerBackend;

class Mixer : public QObject
{
    Q_OBJECT
    Q_PROPERTY(bool available READ isAvailable CONSTANT)
    Q_PROPERTY(QString backendName READ backendName CONSTANT)
    Q_PROPERTY(int master READ master WRITE setMaster NOTIFY masterChanged)
    Q_PROPERTY(bool muted READ isMuted WRITE setMuted NOTIFY mutedChanged)
public:
    Mixer(QObject *parent = 0);
    ~Mixer();

    void initialize();

    bool isAvailable() const;
    QString backendName() const;

    int master() const;
    void setMaster(int value);

    Q_INVOKABLE void increaseMaster();
    Q_INVOKABLE void decreaseMaster();

    bool isMuted() const;
    void setMuted(bool value);

Q_SIGNALS:
    void masterChanged();
    void mutedChanged();

private:
    MixerBackend *m_backend;
    int m_min;
    int m_max;
    int m_step;
};

#endif // MIXER_H
