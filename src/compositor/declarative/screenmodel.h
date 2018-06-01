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

#ifndef SCREENMODEL_H
#define SCREENMODEL_H

#include <QAbstractListModel>
#include <QQmlComponent>
#include <QQmlParserStatus>
#include <QQmlListProperty>
#include <QScreen>
#include <QWaylandOutput>

class ScreenModel : public QAbstractListModel, public QQmlParserStatus
{
    Q_OBJECT
    Q_PROPERTY(QString fileName READ fileName WRITE setFileName NOTIFY fileNameChanged)
    Q_INTERFACES(QQmlParserStatus)
public:
    enum Roles {
        ScreenItemRole = Qt::UserRole,
        ScreenRole,
        PrimaryRole,
        ManufacturerRole,
        ModelRole,
        NameRole,
        XRole,
        YRole,
        WidthRole,
        HeightRole,
        PhysicalSizeRole,
        ScaleFactorRole,
        SubpixelRole,
        TransformRole,
        PowerStateRole,
        ModesRole,
        CurrentModeIndex,
        PreferredModeIndex
    };
    Q_ENUM(Roles)

    explicit ScreenModel(QObject *parent = nullptr);
    ~ScreenModel();

    QString fileName() const;
    void setFileName(const QString &fileName);

    Q_INVOKABLE class ScreenItem *get(int index) const;

    QHash<int, QByteArray> roleNames() const override;
    int rowCount(const QModelIndex &parent = QModelIndex()) const override;
    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;

Q_SIGNALS:
    void fileNameChanged();

protected:
    void classBegin() override;
    void componentComplete() override;

private:
    bool m_initialized = false;
    QString m_fileName;
    QVector<class ScreenItem *> m_items;

    void addFakeScreens();

private Q_SLOTS:
    void handleScreenAdded(QScreen *screen);
};

class ScreenMode : public QObject
{
    Q_OBJECT
    Q_PROPERTY(QSize resolution READ resolution CONSTANT)
    Q_PROPERTY(int refreshRate READ refreshRate CONSTANT)
public:
    explicit ScreenMode(QObject *parent = nullptr);

    QSize resolution() const;
    int refreshRate() const;

private:
    friend class ScreenModel;

    QSize m_resolution;
    int m_refreshRate = 0;
};

QML_DECLARE_TYPE(ScreenMode)

class ScreenItem : public QObject
{
    Q_OBJECT
    Q_PROPERTY(QScreen *screen READ screen CONSTANT)
    Q_PROPERTY(bool primary READ isPrimary NOTIFY primaryChanged)
    Q_PROPERTY(QString manufacturer READ manufacturer CONSTANT)
    Q_PROPERTY(QString model READ model CONSTANT)
    Q_PROPERTY(QString name READ name CONSTANT)
    Q_PROPERTY(int x READ x NOTIFY geometryChanged)
    Q_PROPERTY(int y READ y NOTIFY geometryChanged)
    Q_PROPERTY(int width READ width NOTIFY geometryChanged)
    Q_PROPERTY(int height READ height NOTIFY geometryChanged)
    Q_PROPERTY(QRect geometry READ geometry NOTIFY geometryChanged)
    Q_PROPERTY(QSizeF physicalSize READ physicalSize NOTIFY physicalSizeChanged)
    Q_PROPERTY(int scaleFactor READ scaleFactor CONSTANT)
    Q_PROPERTY(QWaylandOutput::Subpixel subpixel READ subpixel CONSTANT)
    Q_PROPERTY(QWaylandOutput::Transform transform READ transform NOTIFY transformChanged)
    Q_PROPERTY(QQmlListProperty<ScreenMode> modes READ modes CONSTANT)
    Q_PROPERTY(int currentModeIndex READ currentModeIndex NOTIFY currentModeIndexChanged)
    Q_PROPERTY(int preferredModeIndex READ preferredModeIndex CONSTANT)
public:
    explicit ScreenItem(QObject *parent = nullptr);
    ~ScreenItem();

    QScreen *screen() const;

    bool isPrimary() const;

    QString manufacturer() const;
    QString model() const;
    QString name() const;
    int x() const;
    int y() const;
    int width() const;
    int height() const;
    QRect geometry() const;
    QSizeF physicalSize() const;
    int scaleFactor() const;
    QWaylandOutput::Subpixel subpixel() const;
    QWaylandOutput::Transform transform() const;
    QQmlListProperty<ScreenMode> modes();
    int currentModeIndex() const;
    int preferredModeIndex() const;

Q_SIGNALS:
    void primaryChanged();
    void geometryChanged();
    void physicalSizeChanged();
    void transformChanged();
    void currentModeIndexChanged();

private:
    friend class ScreenModel;

    QScreen *m_screen = nullptr;
    bool m_primary = false;
    QString m_manufacturer;
    QString m_model;
    QString m_name;
    QRect m_geometry;
    QSizeF m_physicalSize;
    int m_scaleFactor;
    QWaylandOutput::Subpixel m_subpixel = QWaylandOutput::SubpixelUnknown;
    QWaylandOutput::Transform m_transform = QWaylandOutput::TransformNormal;
    int m_currentMode = 0;
    int m_preferredMode = 0;
    QVector<ScreenMode *> m_modes;
};

QML_DECLARE_TYPE(ScreenItem)

#endif // SCREENMODEL_H
