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

#ifndef LIRI_OUTPUTDEVICE_H
#define LIRI_OUTPUTDEVICE_H

#include <QQmlParserStatus>
#include <QPoint>
#include <QSize>
#include <QObject>
#include <QWaylandOutput>

class OutputDeviceAdaptor;
class OutputManagement;

class OutputDevice : public QObject, public QQmlParserStatus
{
    Q_OBJECT
    Q_PROPERTY(OutputManagement *manager READ manager WRITE setManager NOTIFY managerChanged)
    Q_PROPERTY(QString uuid READ uuid WRITE setUuid NOTIFY uuidChanged)
    Q_PROPERTY(bool enabled READ isEnabled WRITE setEnabled NOTIFY enabledChanged)
    Q_PROPERTY(QString manufacturer READ manufacturer WRITE setManufacturer NOTIFY manufacturerChanged)
    Q_PROPERTY(QString model READ model WRITE setModel NOTIFY modelChanged)
    Q_PROPERTY(QPoint position READ position WRITE setPosition NOTIFY positionChanged)
    Q_PROPERTY(QSize physicalSize READ physicalSize WRITE setPhysicalSize NOTIFY physicalSizeChanged)
    Q_PROPERTY(QWaylandOutput::Transform transform READ transform WRITE setTransform NOTIFY transformChanged)
    Q_PROPERTY(int scaleFactor READ scaleFactor WRITE setScaleFactor NOTIFY scaleFactorChanged)
    Q_PROPERTY(int currentModeIndex READ currentModeIndex WRITE setCurrentModeIndex NOTIFY currentModeIndexChanged)
    Q_PROPERTY(int preferredModeIndex READ preferredModeIndex WRITE setPreferredModeIndex NOTIFY preferredModeIndexChanged)
    Q_INTERFACES(QQmlParserStatus)
public:
    explicit OutputDevice(QObject *parent = nullptr);
    ~OutputDevice();

    OutputManagement *manager() const;
    void setManager(OutputManagement *manager);

    QString uuid() const;
    void setUuid(const QString &uuid);

    bool isEnabled() const;
    void setEnabled(bool enabled);

    QString manufacturer() const;
    void setManufacturer(const QString &manufacturer);

    QString model() const;
    void setModel(const QString &model);

    QPoint position() const;
    void setPosition(const QPoint &position);

    QSize physicalSize() const;
    void setPhysicalSize(const QSize &size);

    QWaylandOutput::Transform transform() const;
    void setTransform(const QWaylandOutput::Transform &transform);

    int scaleFactor() const;
    void setScaleFactor(int scale);

    int currentModeIndex() const;
    void setCurrentModeIndex(int index);

    int preferredModeIndex() const;
    void setPreferredModeIndex(int index);

    Q_INVOKABLE void addMode(const QSize &size, int refreshRate);

Q_SIGNALS:
    void managerChanged();
    void uuidChanged();
    void enabledChanged();
    void manufacturerChanged();
    void modelChanged();
    void positionChanged();
    void physicalSizeChanged();
    void transformChanged();
    void scaleFactorChanged();
    void currentModeIndexChanged();
    void preferredModeIndexChanged();

protected:
    void classBegin() override {}
    void componentComplete() override;

private:
    OutputDeviceAdaptor *m_adaptor = nullptr;
    OutputManagement *m_manager = nullptr;
    bool m_initialized = false;
    QString m_uuid;
    bool m_enabled = true;
    QString m_manufacturer = QStringLiteral("Unknown");
    QString m_model = QStringLiteral("Unknown");
    QPoint m_position;
    QSize m_physicalSize;
    QWaylandOutput::Transform m_transform = QWaylandOutput::TransformNormal;
    int m_scaleFactor = 1;
    QVector<QWaylandOutputMode> m_modes;
    int m_currentModeIndex = 0;
    int m_preferredModexIndex = 0;

    friend class OutputManagement;
    friend class OutputManagementAdaptor;
};

#endif // LIRI_OUTPUTDEVICE_H
