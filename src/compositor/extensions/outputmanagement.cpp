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
#include <QWaylandResource>

#include "extensions/outputconfiguration.h"
#include "extensions/outputconfiguration_p.h"
#include "extensions/outputmanagement.h"
#include "extensions/outputmanagement_p.h"
#include "logging_p.h"

/*
 * OutputManagementPrivate
 */

OutputManagementPrivate::OutputManagementPrivate(OutputManagement *self)
    : QtWaylandServer::liri_outputmanagement()
    , q_ptr(self)
{
}

void OutputManagementPrivate::removeConfiguration(OutputConfiguration *configuration)
{
    configurations.removeOne(configuration);
}

void OutputManagementPrivate::liri_outputmanagement_create_configuration(Resource *resource, uint32_t id)
{
    Q_Q(OutputManagement);

    QWaylandResource configurationResource(
                wl_resource_create(resource->client(),
                                   &liri_outputconfiguration_interface,
                                   wl_resource_get_version(resource->handle),
                                   id));

    Q_EMIT q->createOutputConfiguration(configurationResource);

    OutputConfiguration *configuration = OutputConfiguration::fromResource(configurationResource.resource());
    if (!configuration) {
        // An OutputConfiguration was not created in response to the createOutputConfiguration
        // signal so we create one as a fallback here instead
        configuration = new OutputConfiguration(q, configurationResource);
    }

    Q_EMIT q->outputConfigurationCreated(configuration);

    configurations.append(configuration);
}

/*
 * OutputManagement
 */

OutputManagement::OutputManagement()
    : QWaylandCompositorExtensionTemplate<OutputManagement>()
    , d_ptr(new OutputManagementPrivate(this))
{
}

OutputManagement::OutputManagement(QWaylandCompositor *compositor)
    : QWaylandCompositorExtensionTemplate<OutputManagement>(compositor)
    , d_ptr(new OutputManagementPrivate(this))
{
}

void OutputManagement::initialize()
{
    Q_D(OutputManagement);

    QWaylandCompositorExtensionTemplate::initialize();
    QWaylandCompositor *compositor = static_cast<QWaylandCompositor *>(extensionContainer());
    if (!compositor) {
        qCWarning(lcOutputManagement) << "Failed to find QWaylandCompositor when initializing OutputManagement";
        return;
    }
    d->init(compositor->display(), 1);
}

const struct wl_interface *OutputManagement::interface()
{
    return OutputManagementPrivate::interface();
}

QByteArray OutputManagement::interfaceName()
{
    return OutputManagementPrivate::interfaceName();
}
