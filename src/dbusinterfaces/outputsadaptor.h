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

#ifndef LIRI_OUTPUTSADAPTOR_H
#define LIRI_OUTPUTSADAPTOR_H

#include <QDBusObjectPath>
#include <QObject>
#include <QVector>

class OutputAdaptor : public QObject
{
    Q_OBJECT
    Q_PROPERTY(QString Uuid READ uuid CONSTANT)
    Q_PROPERTY(QString Manufacturer READ manufacturer NOTIFY ManufacturerChanged)
    Q_PROPERTY(QString Model READ model NOTIFY ModelChanged)
    Q_CLASSINFO("D-Bus Interface", "io.liri.Shell.Output")
public:
    explicit OutputAdaptor(const QString &uuid, QObject *parent = nullptr);
    ~OutputAdaptor();

    QDBusObjectPath path() const;

    QString uuid() const;

    QString manufacturer() const;
    void setManufacturer(const QString &manufacturer);

    QString model() const;
    void setModel(const QString &model);

Q_SIGNALS:
    void ManufacturerChanged(const QString &manufacturer);
    void ModelChanged(const QString &model);

private:
    QString m_uuid;
    QString m_manufacturer;
    QString m_model;
};

class OutputsAdaptor : public QObject
{
    Q_OBJECT
    Q_PROPERTY(QDBusObjectPath PrimaryOutput READ primaryOutput NOTIFY PrimaryOutputChanged)
    Q_PROPERTY(QList<QDBusObjectPath> Outputs READ outputs)
    Q_CLASSINFO("D-Bus Interface", "io.liri.Shell.Outputs")
public:
    explicit OutputsAdaptor(QObject *parent = nullptr);
    ~OutputsAdaptor();

    QDBusObjectPath primaryOutput() const;
    void setPrimaryOutput(const QString &uuid);

    QList<QDBusObjectPath> outputs() const;

    void addOutput(OutputAdaptor *outputAdaptor);
    void removeOutput(const QString &uuid);

    bool hasOutput(const QString &uuid);

Q_SIGNALS:
    void PrimaryOutputChanged(const QDBusObjectPath &output);
    void OutputAdded(const QDBusObjectPath &output);
    void OutputRemoved(const QDBusObjectPath &output);

private:
    QDBusObjectPath m_primaryOutputPath;
    QVector<OutputAdaptor *> m_outputs;
};

#endif // LIRI_OUTPUTSADAPTOR_H
