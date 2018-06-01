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

#ifndef LIRI_OUTPUTMANAGEMENT_P_H
#define LIRI_OUTPUTMANAGEMENT_P_H

#include "extensions/outputmanagement.h"
#include "qwayland-server-liri-outputmanagement.h"

//
//  W A R N I N G
//  -------------
//
// This file is not part of the Liri API.  It exists purely as an
// implementation detail.  This header file may change from version to
// version without notice, or even be removed.
//
// We mean it.
//

class OutputManagementPrivate : public QtWaylandServer::liri_outputmanagement
{
    Q_DECLARE_PUBLIC(OutputManagement)
public:
    OutputManagementPrivate(OutputManagement *self);

    void removeConfiguration(OutputConfiguration *configuration);

    QVector<OutputConfiguration *> configurations;

    static OutputManagementPrivate *get(OutputManagement *management) { return management->d_func(); }

protected:
    OutputManagement *q_ptr;

    virtual void liri_outputmanagement_create_configuration(Resource *resource, uint32_t id) override;
};

#endif // LIRI_OUTPUTMANAGEMENT_P_H
