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

#include "extensions/outputchangeset.h"
#include "extensions/outputchangeset_p.h"
#include "extensions/outputconfiguration.h"
#include "extensions/outputconfiguration_p.h"
#include "extensions/outputmanagement.h"
#include "extensions/outputmanagement_p.h"

/*
 * OutputConfigurationPrivate
 */

OutputConfigurationPrivate::OutputConfigurationPrivate(OutputConfiguration *self)
    : QtWaylandServer::liri_outputconfiguration()
    , management(nullptr)
    , q_ptr(self)
{
}

OutputChangeset *OutputConfigurationPrivate::pendingChanges(QWaylandOutput *output)
{
    Q_Q(OutputConfiguration);

    if (!changes.value(output, nullptr))
        changes[output] = new OutputChangeset(output, q);
    return changes[output];
}

bool OutputConfigurationPrivate::hasPendingChanges(QWaylandOutput *output) const
{
    if (!changes.value(output, nullptr))
        return false;

    OutputChangeset *changeset = changes[output];
    return changeset->isEnabledChanged() ||
            changeset->isPrimaryChanged() ||
            changeset->isModeIdChanged() ||
            changeset->isTransformChanged() ||
            changeset->isPositionChanged() ||
            changeset->isScaleFactorChanged();
}

void OutputConfigurationPrivate::clearPendingChanges()
{
    qDeleteAll(changes.begin(), changes.end());
    changes.clear();
}

void OutputConfigurationPrivate::liri_outputconfiguration_enable(Resource *resource,
                                                                 struct ::wl_resource *outputResource,
                                                                 int32_t enable)
{
    Q_UNUSED(resource);
    QWaylandOutput *output = QWaylandOutput::fromResource(outputResource);
    OutputChangesetPrivate::get(pendingChanges(output))->enabled = enable == 1;
}

void OutputConfigurationPrivate::liri_outputconfiguration_primary(Resource *resource,
                                                                  struct ::wl_resource *outputResource,
                                                                  int32_t primary)
{
    Q_UNUSED(resource);
    QWaylandOutput *output = QWaylandOutput::fromResource(outputResource);
    OutputChangesetPrivate::get(pendingChanges(output))->primary = primary == 1;
}

void OutputConfigurationPrivate::liri_outputconfiguration_mode(Resource *resource,
                                                               struct ::wl_resource *outputResource,
                                                               int32_t mode_id)
{
    Q_UNUSED(resource);
    QWaylandOutput *output = QWaylandOutput::fromResource(outputResource);
    OutputChangesetPrivate::get(pendingChanges(output))->modeId = mode_id;
}

void OutputConfigurationPrivate::liri_outputconfiguration_transform(Resource *resource,
                                                                    struct ::wl_resource *outputResource,
                                                                    int32_t wlTransform)
{
    Q_UNUSED(resource);

    QWaylandOutput::Transform transform = static_cast<QWaylandOutput::Transform>(wlTransform);
    QWaylandOutput *output = QWaylandOutput::fromResource(outputResource);
    OutputChangesetPrivate::get(pendingChanges(output))->transform = transform;
}

void OutputConfigurationPrivate::liri_outputconfiguration_position(Resource *resource,
                                                                   struct ::wl_resource *outputResource,
                                                                   int32_t x, int32_t y)
{
    Q_UNUSED(resource);
    QWaylandOutput *output = QWaylandOutput::fromResource(outputResource);
    OutputChangesetPrivate::get(pendingChanges(output))->position = QPoint(x, y);
}

void OutputConfigurationPrivate::liri_outputconfiguration_scale(Resource *resource,
                                                                struct ::wl_resource *outputResource,
                                                                int32_t scale)
{
    Q_UNUSED(resource);
    QWaylandOutput *output = QWaylandOutput::fromResource(outputResource);
    OutputChangesetPrivate::get(pendingChanges(output))->scaleFactor = scale;
}

void OutputConfigurationPrivate::liri_outputconfiguration_apply(Resource *resource)
{
    Q_UNUSED(resource);

    Q_Q(OutputConfiguration);
    Q_EMIT q->changeRequested();
}

/*
 * OutputConfiguration
 */

OutputConfiguration::OutputConfiguration()
    : QWaylandCompositorExtensionTemplate<OutputConfiguration>()
    , d_ptr(new OutputConfigurationPrivate(this))
{
}

OutputConfiguration::OutputConfiguration(OutputManagement *parent, const QWaylandResource &resource)
    : QWaylandCompositorExtensionTemplate<OutputConfiguration>()
    , d_ptr(new OutputConfigurationPrivate(this))
{
    initialize(parent, resource);
}

OutputConfiguration::~OutputConfiguration()
{
    Q_D(OutputConfiguration);
    OutputManagementPrivate::get(d->management)->removeConfiguration(this);
    delete d_ptr;
}

void OutputConfiguration::initialize(OutputManagement *parent, const QWaylandResource &resource)
{
    Q_D(OutputConfiguration);
    d->management = parent;
    d->init(resource.resource());
    setExtensionContainer(parent);
    QWaylandCompositorExtension::initialize();
}

void OutputConfiguration::setApplied()
{
    Q_D(OutputConfiguration);
    d->clearPendingChanges();
    d->send_applied();
}

void OutputConfiguration::setFailed()
{
    Q_D(OutputConfiguration);
    d->clearPendingChanges();
    d->send_failed();
}

QHash<QWaylandOutput *, OutputChangeset *> OutputConfiguration::changes() const
{
    Q_D(const OutputConfiguration);
    return d->changes;
}

const struct wl_interface *OutputConfiguration::interface()
{
    return OutputConfigurationPrivate::interface();
}

QByteArray OutputConfiguration::interfaceName()
{
    return OutputConfigurationPrivate::interfaceName();
}

OutputConfiguration *OutputConfiguration::fromResource(wl_resource *resource)
{
    OutputConfigurationPrivate::Resource *res =
            OutputConfigurationPrivate::Resource::fromResource(resource);
    if (res)
        return static_cast<OutputConfigurationPrivate *>(res->liri_outputconfiguration_object)->q_func();
    return nullptr;
}

void OutputConfiguration::initialize()
{
    QWaylandCompositorExtension::initialize();
}
