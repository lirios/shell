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

#ifndef LIRI_OUTPUTMANAGEMENT_H
#define LIRI_OUTPUTMANAGEMENT_H

#include <QObject>

#include <QWaylandCompositorExtension>
#include <QWaylandResource>

class OutputConfiguration;
class OutputManagementPrivate;

class OutputManagement : public QWaylandCompositorExtensionTemplate<OutputManagement>
{
    Q_OBJECT
    Q_DECLARE_PRIVATE(OutputManagement)
public:
    OutputManagement();
    explicit OutputManagement(QWaylandCompositor *compositor);

    void initialize() override;

    static const struct wl_interface *interface();
    static QByteArray interfaceName();

Q_SIGNALS:
    void createOutputConfiguration(const QWaylandResource &resource);
    void outputConfigurationCreated(OutputConfiguration *configuration);

private:
    OutputManagementPrivate *const d_ptr;
};

#endif // LIRI_OUTPUTMANAGEMENT_H
