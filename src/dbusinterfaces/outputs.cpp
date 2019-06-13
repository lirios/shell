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

#include "outputs.h"
#include "outputsadaptor.h"

Outputs::Outputs(QObject *parent)
    : QObject(parent)
    , m_adaptor(new OutputsAdaptor(this))
{
}

QWaylandCompositor *Outputs::compositor() const
{
    return m_compositor;
}

void Outputs::setCompositor(QWaylandCompositor *compositor)
{
    if (m_compositor == compositor)
        return;

    if (m_compositor) {
        disconnect(m_compositor, &QWaylandCompositor::defaultOutputChanged,
                   this, &Outputs::handleDefaultOutputChanged);
        disconnect(m_compositor, &QWaylandCompositor::outputAdded,
                   this, &Outputs::handleOutputAdded);
        disconnect(m_compositor, &QWaylandCompositor::outputRemoved,
                   this, &Outputs::handleOutputRemoved);
    }

    m_compositor = compositor;
    Q_EMIT compositorChanged();
    connect(m_compositor, &QWaylandCompositor::defaultOutputChanged,
            this, &Outputs::handleDefaultOutputChanged);
    connect(m_compositor, &QWaylandCompositor::outputAdded,
            this, &Outputs::handleOutputAdded);
    connect(m_compositor, &QWaylandCompositor::outputRemoved,
            this, &Outputs::handleOutputRemoved);
}

void Outputs::addOutput(QWaylandOutput *output)
{
    // XXX: It seems that QWaylandCompositor::outputAdded() is never
    // emitted, so we manually add outputs until that is sorted out upstream
    handleOutputAdded(output);
    handleDefaultOutputChanged();
}

void Outputs::componentComplete()
{
    if (!compositor()) {
        for (QObject *p = parent(); p != nullptr; p = p->parent()) {
            if (auto c = qobject_cast<QWaylandCompositor *>(p)) {
                setCompositor(c);
                break;
            }
        }
    }

    for (auto output : compositor()->outputs())
        handleOutputAdded(output);
}

void Outputs::handleDefaultOutputChanged()
{
    if (!compositor() || !compositor()->defaultOutput())
        return;

    QVariant value = compositor()->defaultOutput()->property("uuid");
    if (!value.isValid())
        return;

    QString uuid = value.toString();
    if (uuid.isEmpty())
        return;

    m_adaptor->setPrimaryOutput(uuid);
}

void Outputs::handleOutputAdded(QWaylandOutput *output)
{
    QVariant value = output->property("uuid");
    if (!value.isValid())
        return;

    QString uuid = value.toString();
    if (uuid.isEmpty())
        return;

    if (m_adaptor->hasOutput(uuid))
        return;

    OutputAdaptor *outputAdaptor = new OutputAdaptor(uuid, m_adaptor);
    outputAdaptor->setManufacturer(output->manufacturer());
    outputAdaptor->setModel(output->model());
    connect(output, &QWaylandOutput::manufacturerChanged, this, [output, outputAdaptor] {
        outputAdaptor->setManufacturer(output->manufacturer());
    });
    connect(output, &QWaylandOutput::modelChanged, this, [output, outputAdaptor] {
        outputAdaptor->setModel(output->model());
    });
    m_adaptor->addOutput(outputAdaptor);
}

void Outputs::handleOutputRemoved(QWaylandOutput *output)
{
    QVariant value = output->property("uuid");
    if (!value.isValid())
        return;

    QString uuid = value.toString();
    if (uuid.isEmpty())
        return;

    m_adaptor->removeOutput(uuid);
}
