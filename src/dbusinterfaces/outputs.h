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

#ifndef LIRI_OUTPUTS_H
#define LIRI_OUTPUTS_H

#include <QObject>
#include <QQmlParserStatus>
#include <QWaylandCompositor>

class OutputsAdaptor;

class Outputs : public QObject, public QQmlParserStatus
{
    Q_OBJECT
    Q_PROPERTY(QWaylandCompositor *compositor READ compositor WRITE setCompositor NOTIFY compositorChanged)
    Q_INTERFACES(QQmlParserStatus)
public:
    explicit Outputs(QObject *parent = nullptr);

    QWaylandCompositor *compositor() const;
    void setCompositor(QWaylandCompositor *compositor);

    Q_INVOKABLE void addOutput(QWaylandOutput *output);


Q_SIGNALS:
    void compositorChanged();

protected:
    void classBegin() override {}
    void componentComplete() override;

private:
    OutputsAdaptor *m_adaptor = nullptr;
    QWaylandCompositor *m_compositor = nullptr;

private Q_SLOTS:
    void handleDefaultOutputChanged();
    void handleOutputAdded(QWaylandOutput *output);
    void handleOutputRemoved(QWaylandOutput *output);
};

#endif // LIRI_OUTPUTS_H
