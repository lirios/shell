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

#include <QWaylandCompositor>

#include "extensions/outputchangeset.h"
#include "extensions/outputchangeset_p.h"

/*
 * OutputChangesetPrivate
 */

OutputChangesetPrivate::OutputChangesetPrivate(QWaylandOutput *output)
    : output(output)
    // TODO: Implement enabled
    //, enabled(output->isEnabled())
    , enabled(true)
    , primary(output->compositor()->defaultOutput() == output)
    , transform(output->transform())
    , modeId(output->modes().indexOf(output->currentMode()))
    , position(output->position())
    , scaleFactor(output->scaleFactor())
{
}

/*
 * OutputChangeset
 */

OutputChangeset::OutputChangeset(QWaylandOutput *output, QObject *parent)
    : QObject(parent)
    , d_ptr(new OutputChangesetPrivate(output))
{
}

OutputChangeset::~OutputChangeset()
{
    delete d_ptr;
}

QWaylandOutput *OutputChangeset::output() const
{
    Q_D(const OutputChangeset);
    return d->output;
}

bool OutputChangeset::isEnabledChanged() const
{
    // TODO: Uncomment when enabled is implemented
    return false;
    //return d->enabled != d->output->isEnabled();
}

bool OutputChangeset::isPrimaryChanged() const
{
    Q_D(const OutputChangeset);
    return d->primary != (d->output->compositor()->defaultOutput() == d->output);
}

bool OutputChangeset::isTransformChanged() const
{
    Q_D(const OutputChangeset);
    return d->transform != d->output->transform();
}

bool OutputChangeset::isModeIdChanged() const
{
    Q_D(const OutputChangeset);
    return d->modeId != d->output->modes().indexOf(d->output->currentMode());
}

bool OutputChangeset::isPositionChanged() const
{
    Q_D(const OutputChangeset);
    return d->position != d->output->position();
}

bool OutputChangeset::isScaleFactorChanged() const
{
    Q_D(const OutputChangeset);
    return d->scaleFactor != d->output->scaleFactor();
}

bool OutputChangeset::isEnabled() const
{
    Q_D(const OutputChangeset);
    return d->enabled;
}

bool OutputChangeset::isPrimary() const
{
    Q_D(const OutputChangeset);
    return d->primary;
}

int OutputChangeset::modeId() const
{
    Q_D(const OutputChangeset);
    return d->modeId;
}

QWaylandOutput::Transform OutputChangeset::transform() const
{
    Q_D(const OutputChangeset);
    return d->transform;
}

QPoint OutputChangeset::position() const
{
    Q_D(const OutputChangeset);
    return d->position;
}

int OutputChangeset::scaleFactor() const
{
    Q_D(const OutputChangeset);
    return d->scaleFactor;
}
