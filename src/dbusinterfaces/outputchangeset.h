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

#ifndef LIRI_OUTPUTCHANGESET_H
#define LIRI_OUTPUTCHANGESET_H

#include <QObject>
#include <QWaylandOutput>

class OutputConfiguration;
class OutputConfigurationAdaptor;
class OutputDevice;

class OutputChangeset : public QObject
{
    Q_OBJECT
    Q_PROPERTY(bool empty READ isEmpty CONSTANT)
    Q_PROPERTY(OutputDevice *outputDevice READ outputDevice CONSTANT)
    Q_PROPERTY(bool enabledChanged READ isEnabledChanged CONSTANT)
    Q_PROPERTY(bool transformChanged READ isTransformChanged CONSTANT)
    Q_PROPERTY(bool currentModeIndexChanged READ isCurrentModeIndexChanged CONSTANT)
    Q_PROPERTY(bool positionChanged READ isPositionChanged CONSTANT)
    Q_PROPERTY(bool scaleFactorChanged READ isScaleFactorChanged CONSTANT)
    Q_PROPERTY(bool enabled READ isEnabled CONSTANT)
    Q_PROPERTY(int currentModeIndex READ currentModeIndex CONSTANT)
    Q_PROPERTY(QWaylandOutput::Transform transform READ transform CONSTANT)
    Q_PROPERTY(QPoint position READ position CONSTANT)
    Q_PROPERTY(int scaleFactor READ scaleFactor CONSTANT)
public:
    bool isEmpty() const;

    OutputDevice *outputDevice() const;

    bool isEnabledChanged() const;
    bool isTransformChanged() const;
    bool isCurrentModeIndexChanged() const;
    bool isPositionChanged() const;
    bool isScaleFactorChanged() const;

    bool isEnabled() const;
    void setEnabled(bool enabled);

    int currentModeIndex() const;
    void setCurrentModeIndex(int index);

    QWaylandOutput::Transform transform() const;
    void setTransform(QWaylandOutput::Transform transform);

    QPoint position() const;
    void setPosition(const QPoint &position);

    int scaleFactor() const;
    void setScaleFactor(int scaleFactor);

private:
    OutputDevice *m_outputDevice = nullptr;
    bool m_enabled = true;
    int m_currentModeIndex = -1;
    QWaylandOutput::Transform m_transform = QWaylandOutput::TransformNormal;
    QPoint m_position;
    int m_scaleFactor = 1;

    explicit OutputChangeset(OutputDevice *output, QObject *parent = nullptr);

    friend class OutputConfiguration;
    friend class OutputConfigurationAdaptor;
};

#endif // LIRI_OUTPUTCHANGESET_H
