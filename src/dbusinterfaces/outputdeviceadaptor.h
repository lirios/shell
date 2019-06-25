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

#ifndef LIRI_OUTPUTDEVICEADAPTOR_H
#define LIRI_OUTPUTDEVICEADAPTOR_H

#include <QDBusArgument>
#include <QDBusObjectPath>
#include <QPoint>
#include <QSize>
#include <QObject>
#include <QWaylandOutputMode>

class OutputDevice;

struct Mode {
    QString name;
    QSize size;
    int refreshRate;
};
Q_DECLARE_METATYPE(Mode)

class OutputDeviceAdaptor : public QObject
{
    Q_OBJECT
    Q_PROPERTY(QString Uuid READ uuid CONSTANT)
    Q_PROPERTY(bool Enabled READ isEnabled NOTIFY EnabledChanged)
    Q_PROPERTY(QString Manufacturer READ manufacturer NOTIFY ManufacturerChanged)
    Q_PROPERTY(QString Model READ model NOTIFY ModelChanged)
    Q_PROPERTY(QPoint Position READ position NOTIFY PositionChanged)
    Q_PROPERTY(QSize PhysicalSize READ physicalSize NOTIFY PhysicalSizeChanged)
    Q_PROPERTY(QString Transform READ transform NOTIFY TransformChanged)
    Q_PROPERTY(int ScaleFactor READ scaleFactor NOTIFY ScaleFactorChanged)
    Q_PROPERTY(QList<Mode> Modes READ modes NOTIFY ModeAdded)
    Q_PROPERTY(int CurrentModeIndex READ currentModeIndex NOTIFY CurrentModeIndexChanged)
    Q_PROPERTY(int PreferredModeIndex READ preferredModeIndex NOTIFY PreferredModeIndexChanged)
    Q_CLASSINFO("D-Bus Interface", "io.liri.Shell.OutputDevice1")
public:
    explicit OutputDeviceAdaptor(const QString &uuid, OutputDevice *parent);
    ~OutputDeviceAdaptor();

    QDBusObjectPath path() const;

    QString uuid() const;

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

    QString transform() const;
    void setTransform(const QString &transform);

    int scaleFactor() const;
    void setScaleFactor(int scale);

    QList<Mode> modes() const;

    int currentModeIndex() const;
    void setCurrentModeIndex(int index);

    int preferredModeIndex() const;
    void setPreferredModeIndex(int index);

Q_SIGNALS:
    void EnabledChanged(bool enabled);
    void ManufacturerChanged(const QString &manufacturer);
    void ModelChanged(const QString &model);
    void PositionChanged(const QPoint &position);
    void PhysicalSizeChanged(const QSize &size);
    void TransformChanged(const QString &transform);
    void ScaleFactorChanged(int scaleFactor);
    void ModeAdded(const QSize &size, int refreshRate);
    void CurrentModeIndexChanged(int currentModeIndex);
    void PreferredModeIndexChanged(int preferredModeIndex);
    void Changed();

private:
    OutputDevice *m_parent = nullptr;
    QString m_uuid;
    bool m_enabled = true;
    QString m_manufacturer = QStringLiteral("Unknown");
    QString m_model = QStringLiteral("Unknown");
    QPoint m_position;
    QSize m_physicalSize;
    QString m_transform = QStringLiteral("normal");
    int m_scaleFactor = 1;
    QList<Mode> m_modes;
    int m_currentModeIndex = 0;
    int m_preferredModexIndex = 0;

    void addMode(const QSize &size, int refreshRate);

    friend class OutputDevice;
};

inline QDBusArgument &operator<<(QDBusArgument &argument, const Mode &mode)
{
    argument.beginStructure();
    argument << mode.name;
    argument << mode.size;
    argument << mode.refreshRate;
    argument.endStructure();
    return argument;
}

inline const QDBusArgument &operator>>(const QDBusArgument &argument, Mode &mode)
{
    argument.beginStructure();
    argument >> mode.name;
    argument >> mode.size;
    argument >> mode.refreshRate;
    argument.endStructure();
    return argument;
}

#endif // LIRI_OUTPUTDEVICEADAPTOR_H
