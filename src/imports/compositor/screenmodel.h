// SPDX-FileCopyrightText: 2018 Pier Luigi Fiorini <pierluigi.fiorini@gmail.com>
//
// SPDX-License-Identifier: GPL-3.0-or-later

#ifndef SCREENMODEL_H
#define SCREENMODEL_H

#include <QAbstractListModel>
#include <QQmlComponent>
#include <QQmlParserStatus>
#include <QQmlListProperty>
#include <QScreen>

#include <LiriAuroraCompositor/WaylandOutput>
#include <LiriAuroraCompositor/WaylandWlrOutputManagerV1>

using namespace Aurora::Compositor;

class ScreenModel : public QAbstractListModel, public QQmlParserStatus
{
    Q_OBJECT
    Q_PROPERTY(QString fileName READ fileName WRITE setFileName NOTIFY fileNameChanged)
    Q_PROPERTY(int count READ count NOTIFY countChanged)
    Q_INTERFACES(QQmlParserStatus)
public:
    enum Roles {
        ScreenItemRole = Qt::UserRole,
        ScreenRole,
        ScreenIndexRole
    };
    Q_ENUM(Roles)

    explicit ScreenModel(QObject *parent = nullptr);
    ~ScreenModel() override;

    QString fileName() const;
    void setFileName(const QString &fileName);

    int count() const;

    Q_INVOKABLE class ScreenItem *get(int index) const;

    QHash<int, QByteArray> roleNames() const override;
    int rowCount(const QModelIndex &parent = QModelIndex()) const override;
    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;

    Q_INVOKABLE bool testConfiguration(Aurora::Compositor::WaylandWlrOutputConfigurationV1 *configuration);
    Q_INVOKABLE void applyConfiguration(Aurora::Compositor::WaylandWlrOutputConfigurationV1 *configuration);

Q_SIGNALS:
    void fileNameChanged();
    void countChanged();

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

    bool operator==(const ScreenMode &other) const;

private:
    friend class ScreenModel;

    QSize m_resolution;
    int m_refreshRate = 60;
};

QML_DECLARE_TYPE(ScreenMode)

class ScreenItem : public QObject
{
    Q_OBJECT
    Q_PROPERTY(QScreen *screen READ screen CONSTANT)
    Q_PROPERTY(int screenIndex READ screenIndex CONSTANT)
    Q_PROPERTY(bool enabled READ isEnabled NOTIFY enabledChanged)
    Q_PROPERTY(QString manufacturer READ manufacturer CONSTANT)
    Q_PROPERTY(QString model READ model CONSTANT)
    Q_PROPERTY(QString name READ name CONSTANT)
    Q_PROPERTY(QString description READ description CONSTANT)
    Q_PROPERTY(QPoint position READ position NOTIFY positionChanged)
    Q_PROPERTY(QSizeF physicalSize READ physicalSize NOTIFY physicalSizeChanged)
    Q_PROPERTY(qreal scaleFactor READ scaleFactor NOTIFY scaleFactorChanged)
    Q_PROPERTY(Aurora::Compositor::WaylandOutput::Subpixel subpixel READ subpixel CONSTANT)
    Q_PROPERTY(Aurora::Compositor::WaylandOutput::Transform transform READ transform NOTIFY transformChanged)
    Q_PROPERTY(QQmlListProperty<ScreenMode> modes READ modesList CONSTANT)
    Q_PROPERTY(ScreenMode *currentMode READ currentMode NOTIFY currentModeChanged)
    Q_PROPERTY(ScreenMode *preferredMode READ preferredMode CONSTANT)
    Q_PROPERTY(PowerState powerState READ powerState WRITE setPowerState NOTIFY powerStateChanged)
public:
    enum PowerState {
        PowerStateOn,
        PowerStateStandby,
        PowerStateSuspend,
        PowerStateOff
    };
    Q_ENUM(PowerState)

    explicit ScreenItem(QObject *parent = nullptr);
    ~ScreenItem();

    QScreen *screen() const;
    int screenIndex() const;

    bool isEnabled() const;

    QString manufacturer() const;
    QString model() const;
    QString name() const;
    QString description() const;

    QPoint position() const;

    QSizeF physicalSize() const;

    qreal scaleFactor() const;

    Aurora::Compositor::WaylandOutput::Subpixel subpixel() const;
    Aurora::Compositor::WaylandOutput::Transform transform() const;

    QVector<ScreenMode *> modes() const;
    QQmlListProperty<ScreenMode> modesList();

    ScreenMode *currentMode() const;
    ScreenMode *preferredMode() const;

    PowerState powerState() const;
    void setPowerState(PowerState state);

Q_SIGNALS:
    void enabledChanged();
    void positionChanged();
    void physicalSizeChanged();
    void scaleFactorChanged();
    void transformChanged();
    void currentModeChanged(const QSize &resolution, int refreshRate);
    void powerStateChanged();

private:
    friend class ScreenModel;

    QScreen *m_screen = nullptr;
    bool m_enabled = true;
    QString m_manufacturer;
    QString m_model;
    QString m_name;
    QString m_description;
    QPoint m_position;
    QSizeF m_physicalSize;
    qreal m_scaleFactor = 1.0f;
    Aurora::Compositor::WaylandOutput::Subpixel m_subpixel = Aurora::Compositor::WaylandOutput::SubpixelUnknown;
    Aurora::Compositor::WaylandOutput::Transform m_transform = Aurora::Compositor::WaylandOutput::TransformNormal;
    ScreenMode *m_currentMode = nullptr;
    ScreenMode *m_preferredMode = nullptr;
    QVector<ScreenMode *> m_modes;
};

QML_DECLARE_TYPE(ScreenItem)

#endif // SCREENMODEL_H
