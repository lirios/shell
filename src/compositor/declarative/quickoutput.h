/****************************************************************************
 * This file is part of Liri.
 *
 * Copyright (C) 2018 Pier Luigi Fiorini <pierluigi.fiorini@gmail.com>
 *
 * $BEGIN_LICENSE:LGPLv3+$
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
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

#ifndef QUICKOUTPUT_H
#define QUICKOUTPUT_H

#include <QQmlListProperty>
#include <QWaylandQuickOutput>

class ScreenMode;

class QuickOutput : public QWaylandQuickOutput
{
    Q_OBJECT
    Q_PROPERTY(QQmlListProperty<ScreenMode> modes READ screenModes NOTIFY modesChanged)
    Q_PROPERTY(int currentModeIndex READ currentModeIndex WRITE setCurrentModeIndex NOTIFY currentModeIndexChanged)
    Q_PROPERTY(int preferredModeIndex READ preferredModeIndex WRITE setPreferredModeIndex NOTIFY preferredModeIndexChanged)
public:
    explicit QuickOutput();

    QQmlListProperty<ScreenMode> screenModes();

    int currentModeIndex() const;
    void setCurrentModeIndex(int index);

    int preferredModeIndex() const;
    void setPreferredModeIndex(int index);

Q_SIGNALS:
    void modesChanged();
    void currentModeIndexChanged();
    void preferredModeIndexChanged();

protected:
    void initialize() override;

private:
    bool m_initialized = false;
    QVector<ScreenMode *> m_modes;
    int m_currentModeIndex = 0;
    int m_preferredModexIndex = 0;
};

#endif // QUICKOUTPUT_H
