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

class OutputChangesetPrivate;
class OutputConfigurationPrivate;

class OutputChangeset : public QObject
{
    Q_OBJECT
    Q_DECLARE_PRIVATE(OutputChangeset)
    Q_PROPERTY(QWaylandOutput *output READ output CONSTANT)
    Q_PROPERTY(bool enabledChanged READ isEnabledChanged CONSTANT)
    Q_PROPERTY(bool primaryChanged READ isPrimaryChanged CONSTANT)
    Q_PROPERTY(bool transformChanged READ isTransformChanged CONSTANT)
    Q_PROPERTY(bool modeIdChanged READ isModeIdChanged CONSTANT)
    Q_PROPERTY(bool positionChanged READ isPositionChanged CONSTANT)
    Q_PROPERTY(bool scaleFactorChanged READ isScaleFactorChanged CONSTANT)
    Q_PROPERTY(bool enabled READ isEnabled CONSTANT)
    Q_PROPERTY(bool primary READ isPrimary CONSTANT)
    Q_PROPERTY(int modeId READ modeId CONSTANT)
    Q_PROPERTY(QWaylandOutput::Transform transform READ transform CONSTANT)
    Q_PROPERTY(QPoint position READ position CONSTANT)
    Q_PROPERTY(int scaleFactor READ scaleFactor CONSTANT)
public:
    ~OutputChangeset();

    QWaylandOutput *output() const;

    bool isEnabledChanged() const;
    bool isPrimaryChanged() const;
    bool isTransformChanged() const;
    bool isModeIdChanged() const;
    bool isPositionChanged() const;
    bool isScaleFactorChanged() const;

    bool isEnabled() const;
    bool isPrimary() const;
    int modeId() const;
    QWaylandOutput::Transform transform() const;
    QPoint position() const;
    int scaleFactor() const;

private:
    explicit OutputChangeset(QWaylandOutput *output, QObject *parent = nullptr);

    friend class OutputConfigurationPrivate;

    OutputChangesetPrivate *const d_ptr;
};

#endif // LIRI_OUTPUTCHANGESET_H
