// SPDX-FileCopyrightText: 2018 Pier Luigi Fiorini <pierluigi.fiorini@gmail.com>
//
// SPDX-License-Identifier: GPL-3.0-or-later

#include <QFile>
#include <QGuiApplication>
#include <QJsonArray>
#include <QJsonDocument>
#include <QJsonObject>
#include <QtMath>
#include <qpa/qplatformscreen.h>

#include <LiriAuroraPlatformHeaders/lirieglfsfunctions.h>

#include "logging.h"
#include "screenmodel.h"

static ScreenItem::PowerState convertPowerState(Aurora::PlatformSupport::EglFSFunctions::PowerState powerState)
{
    switch (powerState) {
    case Aurora::PlatformSupport::EglFSFunctions::PowerStateOn:
        return ScreenItem::PowerStateOn;
    case Aurora::PlatformSupport::EglFSFunctions::PowerStateStandby:
        return ScreenItem::PowerStateStandby;
    case Aurora::PlatformSupport::EglFSFunctions::PowerStateSuspend:
        return ScreenItem::PowerStateSuspend;
    case Aurora::PlatformSupport::EglFSFunctions::PowerStateOff:
        return ScreenItem::PowerStateOff;
    }
    Q_UNREACHABLE();
}

static Aurora::PlatformSupport::EglFSFunctions::PowerState convertPowerState(ScreenItem::PowerState powerState)
{
    switch (powerState) {
    case ScreenItem::PowerStateOn:
        return Aurora::PlatformSupport::EglFSFunctions::PowerStateOn;
    case ScreenItem::PowerStateStandby:
        return Aurora::PlatformSupport::EglFSFunctions::PowerStateStandby;
    case ScreenItem::PowerStateSuspend:
        return Aurora::PlatformSupport::EglFSFunctions::PowerStateSuspend;
    case ScreenItem::PowerStateOff:
        return Aurora::PlatformSupport::EglFSFunctions::PowerStateOff;
    }
    Q_UNREACHABLE();
}

/*
 * ScreenMode
 */


ScreenMode::ScreenMode(QObject *parent)
    : QObject(parent)
{
}

QSize ScreenMode::resolution() const
{
    return m_resolution;
}

int ScreenMode::refreshRate() const
{
    return m_refreshRate;
}

bool ScreenMode::operator==(const ScreenMode &other) const
{
    return resolution() == other.resolution() &&
            refreshRate() == other.refreshRate();
}

/*
 * ScreenItem
 */

ScreenItem::ScreenItem(QObject *parent)
    : QObject(parent)
{
}

ScreenItem::~ScreenItem()
{
    qDeleteAll(m_modes);
}

QScreen *ScreenItem::screen() const
{
    return m_screen;
}

int ScreenItem::screenIndex() const
{
    return qGuiApp->screens().indexOf(m_screen);
}

bool ScreenItem::isEnabled() const
{
    return m_enabled;
}

QString ScreenItem::manufacturer() const
{
    return m_manufacturer;
}

QString ScreenItem::model() const
{
    return m_model;
}

QString ScreenItem::name() const
{
    return m_name;
}

QString ScreenItem::description() const
{
    return m_description;
}

QPoint ScreenItem::position() const
{
    return m_position;
}

QSizeF ScreenItem::physicalSize() const
{
    return m_physicalSize;
}

qreal ScreenItem::scaleFactor() const
{
    return m_scaleFactor;
}

WaylandOutput::Subpixel ScreenItem::subpixel() const
{
    return m_subpixel;
}

WaylandOutput::Transform ScreenItem::transform() const
{
    return m_transform;
}

QVector<ScreenMode *> ScreenItem::modes() const
{
    return m_modes;
}

QQmlListProperty<ScreenMode> ScreenItem::modesList()
{
    auto countFunc = [](QQmlListProperty<ScreenMode> *prop) {
        return static_cast<ScreenItem *>(prop->object)->m_modes.count();
    };
    auto atFunc = [](QQmlListProperty<ScreenMode> *prop, int index) {
        return static_cast<ScreenItem *>(prop->object)->m_modes.at(index);
    };
    return QQmlListProperty<ScreenMode>(this, this, countFunc, atFunc);
}

ScreenMode *ScreenItem::currentMode() const
{
    return m_currentMode;
}

ScreenMode *ScreenItem::preferredMode() const
{
    return m_preferredMode;
}

ScreenItem::PowerState ScreenItem::powerState() const
{
    if (!m_screen) {
        qCWarning(lcShell) << "ScreenItem cannot get power state if the screen property is not set";
        return ScreenItem::PowerStateOn;
    }

    return convertPowerState(Aurora::PlatformSupport::EglFSFunctions::getPowerState(m_screen));
}

void ScreenItem::setPowerState(ScreenItem::PowerState state)
{
    if (!m_screen) {
        qCWarning(lcShell) << "ScreenItem cannot set power state if the screen property is not set";
        return;
    }

    auto oldPowerState = Aurora::PlatformSupport::EglFSFunctions::getPowerState(m_screen);
    auto newPowerState = convertPowerState(state);

    if (oldPowerState == newPowerState)
        return;

    Aurora::PlatformSupport::EglFSFunctions::setPowerState(m_screen, newPowerState);

    Q_EMIT powerStateChanged();
}

/*
 * ScreenModel
 */

ScreenModel::ScreenModel(QObject *parent)
    : QAbstractListModel(parent)
{
}

ScreenModel::~ScreenModel()
{
    qDeleteAll(m_items);
}

QString ScreenModel::fileName() const
{
    return m_fileName;
}

void ScreenModel::setFileName(const QString &fileName)
{
    if (fileName == m_fileName)
        return;

    // We can feed the model only once
    if (m_initialized) {
        qCWarning(lcShell, "Screen configuration cannot be changed after initialization");
        return;
    }

    m_fileName = fileName;
    Q_EMIT fileNameChanged();
}

int ScreenModel::count() const
{
    return rowCount();
}

ScreenItem *ScreenModel::get(int index) const
{
    return m_items.at(index);
}

QHash<int, QByteArray> ScreenModel::roleNames() const
{
    QHash<int, QByteArray> roles;
    roles.insert(ScreenItemRole, QByteArrayLiteral("screenItem"));
    roles.insert(ScreenRole, QByteArrayLiteral("screen"));
    roles.insert(ScreenIndexRole, QByteArrayLiteral("screenIndex"));
    return roles;
}

int ScreenModel::rowCount(const QModelIndex &parent) const
{
    Q_UNUSED(parent);
    return m_items.count();
}

QVariant ScreenModel::data(const QModelIndex &index, int role) const
{
    if (!index.isValid())
        return QVariant();

    ScreenItem *item = m_items.at(index.row());

    switch (role) {
    case ScreenItemRole:
        return QVariant::fromValue(item);
    case ScreenRole:
        return QVariant::fromValue<QScreen *>(item->screen());
    case ScreenIndexRole:
        return item->screenIndex();
    }

    return QVariant();
}

bool ScreenModel::testConfiguration(WaylandWlrOutputConfigurationV1 *configuration)
{
    if (!m_fileName.isEmpty()) {
        qCWarning(lcShell, "Cannot test output configuration when using a fake screen backend");
        return false;
    }

    const auto changesList = configuration->enabledHeads();
    for (auto *changes : changesList) {
        for (auto *item : qAsConst(m_items)) {
            if (item->name() == changes->head()->name()) {
                QSize size;
                quint32 refresh = 0;

                if (changes->customModeSize().isValid() && changes->customModeRefresh() > 0) {
                    size = changes->customModeSize();
                    refresh = changes->customModeRefresh();
                } else if (changes->mode()) {
                    size = changes->mode()->size();
                    refresh = changes->mode()->refresh();
                }

                bool modeFound = false;
                const auto modes = item->modes();
                for (auto *screenMode : modes) {
                    if (screenMode->resolution() == size && screenMode->refreshRate() == refresh) {
                        modeFound = true;
                        break;
                    }
                }

                if (!modeFound) {
                    qCWarning(lcShell, "Output configuration test failed: mode not found");
                    return false;
                }

                break;
            }
        }
    }

    return true;
}

void ScreenModel::applyConfiguration(WaylandWlrOutputConfigurationV1 *configuration)
{
    if (!m_fileName.isEmpty()) {
        qCWarning(lcShell, "Cannot test or apply output configuration when using a fake screen backend");
        return;
    }

    QVector<Aurora::PlatformSupport::ScreenChange> screenChanges;

    const auto changesList = configuration->enabledHeads();
    for (auto *changes : changesList) {
        for (auto *item : qAsConst(m_items)) {
            if (item->name() == changes->head()->name()) {
                Aurora::PlatformSupport::ScreenChange screenChange;
                screenChange.screen = item->screen();
                screenChange.enabled = true;
                screenChange.position = changes->position();
                if (changes->customModeSize().isValid() && changes->customModeRefresh() > 0) {
                    screenChange.resolution = changes->customModeSize();
                    screenChange.refreshRate = changes->customModeRefresh();
                } else if (changes->mode()) {
                    screenChange.resolution = changes->mode()->size();
                    screenChange.refreshRate = changes->mode()->refresh();
                }
                screenChange.scale = changes->scale();
                screenChanges.append(screenChange);

                if (!item->isEnabled()) {
                    item->m_enabled = true;
                    Q_EMIT item->enabledChanged();
                }

                break;
            }
        }
    }

    const auto heads = configuration->disabledHeads();
    for (auto *head : heads) {
        for (auto *item : qAsConst(m_items)) {
            if (item->name() == head->name()) {
                Aurora::PlatformSupport::ScreenChange screenChange;
                screenChange.screen = item->screen();
                screenChange.enabled = false;
                screenChanges.append(screenChange);

                if (item->isEnabled()) {
                    item->m_enabled = false;
                    Q_EMIT item->enabledChanged();
                }

                break;
            }
        }
    }

    bool result = Aurora::PlatformSupport::EglFSFunctions::applyScreenChanges(screenChanges);

    // TODO: Send cancelled if a new screen is added or changed meanwhile

    if (result)
        configuration->sendSucceeded();
    else
        configuration->sendFailed();
}

void ScreenModel::classBegin()
{
}

void ScreenModel::componentComplete()
{
    // When the component is complete we decide if we want to feed
    // the model with actual screens or the JSON configuration
    // provided by the user

    if (m_fileName.isEmpty()) {
        const auto screens = qGuiApp->screens();
        for (auto *screen : screens)
            handleScreenAdded(screen);
        Q_EMIT countChanged();

        connect(qGuiApp, &QGuiApplication::screenAdded, this, &ScreenModel::handleScreenAdded);
        connect(qGuiApp, &QGuiApplication::screenRemoved, this, [this](QScreen *screen) {
            int row = 0;
            auto it = m_items.begin();
            while (it != m_items.end()) {
                auto screenItem = (*it);
                if (screenItem->screen() == screen) {
                    beginRemoveRows(QModelIndex(), row, row);
                    it = m_items.erase(it);
                    endRemoveRows();
                    screenItem->deleteLater();
                } else {
                    ++it;
                }
                row++;
            }
        });
    } else {
        addFakeScreens();
    }
}

void ScreenModel::addFakeScreens()
{
    QFile file(m_fileName);
    if (!file.open(QFile::ReadOnly)) {
        qCWarning(lcShell) << "Could not open configuration file"
                           << m_fileName << "for reading";
        return;
    }

    QByteArray data = file.readAll();

    file.close();

    const QJsonDocument doc = QJsonDocument::fromJson(data);
    if (!doc.isObject()) {
        qCWarning(lcShell, "Error parsing \"%s\": no top-level JSON object",
                  qPrintable(m_fileName));
        return;
    }

    const QJsonObject object = doc.object();
    const QJsonArray outputs = object.value(QStringLiteral("outputs")).toArray();

    for (int i = 0; i < outputs.size(); i++) {
        const QVariantMap outputSettings = outputs.at(i).toObject().toVariantMap();
        qCDebug(lcShell) << "Output settings:" << outputSettings;

        QString name = outputSettings.value(QStringLiteral("name")).toString();
        if (name.isEmpty()) {
            if (QGuiApplication::platformName() == QLatin1String("xcb"))
                name = QStringLiteral("X11-");
            else if (QGuiApplication::platformName().contains(QLatin1String("wayland")))
                name = QStringLiteral("WL-");
            else
                name = QStringLiteral("UNK-");
            name.append(QString::number(i));
        }
        qCDebug(lcShell) << "Output name:" << name;

        QString description = outputSettings.value(QStringLiteral("description")).toString();
        if (description.isEmpty()) {
            if (QGuiApplication::platformName() == QLatin1String("xcb"))
                description = QStringLiteral("Virtual X11 output via :%1").arg(QString::fromLocal8Bit(qgetenv("DISPLAY")));
            else if (QGuiApplication::platformName().contains(QLatin1String("wayland")))
                description = QStringLiteral("Virtual Wayland output");
            else
                description = QStringLiteral("Virtual output");
        }
        qCDebug(lcShell) << "Output description:" << description;

        int scale = qMax<int>(1, outputSettings.value(QStringLiteral("scale")).toInt());
        qCDebug(lcShell) << "Scale:" << scale;

        const QVariantMap posValue = outputSettings.value(QStringLiteral("position")).toMap();
        int x = posValue.value(QStringLiteral("x")).toInt();
        int y = posValue.value(QStringLiteral("y")).toInt();
        QPoint pos(x, y);
        qCDebug(lcShell) << "Output position:" << pos;

        const QVariantMap modeValue = outputSettings.value(QStringLiteral("mode")).toMap();
        const QVariantMap sizeValue = modeValue.value(QStringLiteral("size")).toMap();
        int w = sizeValue.value(QStringLiteral("width")).toInt();
        int h = sizeValue.value(QStringLiteral("height")).toInt();
        QSize size = QSize(w, h);
        int refreshRate = modeValue.value(QStringLiteral("refreshRate")).toInt();
        qCDebug(lcShell) << "Output size:" << size;
        qCDebug(lcShell) << "Output refresh rate:" << refreshRate;

        const QVariantMap physicalSizeValue = outputSettings.value(QStringLiteral("physicalSize")).toMap();
        QSizeF physicalSize;
        physicalSize.setWidth(physicalSizeValue.value(QStringLiteral("width"), -1).toInt());
        physicalSize.setHeight(physicalSizeValue.value(QStringLiteral("height"), -1).toInt());
        if (!physicalSize.isValid()) {
            physicalSize.setWidth(w * 0.26458);
            physicalSize.setHeight(h * 0.26458);
        }
        qCDebug(lcShell) << "Physical size millimiters:" << physicalSize;

        Qt::ScreenOrientation orientation =
                static_cast<Qt::ScreenOrientation>(outputSettings.value(QStringLiteral("orientation")).toInt());
        qCDebug(lcShell) << "Output orientation:" << orientation;

        beginInsertRows(QModelIndex(), m_items.count(), m_items.count());

        ScreenItem *item = new ScreenItem(this);
        item->m_enabled = true;
        item->m_manufacturer = QStringLiteral("Liri");
        item->m_model = name;
        item->m_name = name;
        item->m_description = description;
        if (physicalSize.isValid())
            item->m_physicalSize = physicalSize;
        item->m_position = pos;
        item->m_scaleFactor = scale;

        switch (orientation) {
        case Qt::PortraitOrientation:
            item->m_transform = WaylandOutput::Transform90;
            break;
        case Qt::InvertedLandscapeOrientation:
            item->m_transform = WaylandOutput::Transform180;
            break;
        case Qt::InvertedPortraitOrientation:
            item->m_transform = WaylandOutput::Transform270;
            break;
        default:
            break;
        }

        ScreenMode *mode = new ScreenMode(item);
        mode->m_resolution = size;
        mode->m_refreshRate = refreshRate;
        item->m_modes.append(mode);

        m_items.append(item);

        endInsertRows();
    }

    Q_EMIT countChanged();
}

void ScreenModel::handleScreenAdded(QScreen *screen)
{
    beginInsertRows(QModelIndex(), m_items.count(), m_items.count());

    ScreenItem *item = new ScreenItem(this);
    item->m_screen = screen;

    item->m_manufacturer = screen->manufacturer();
    if (item->m_manufacturer.isEmpty())
        item->m_manufacturer = QStringLiteral("Unknown");

    if (!screen->model().isEmpty()) {
        item->m_model = screen->model();
        if (!screen->serialNumber().isEmpty()) {
            item->m_model.append(QLatin1Char('/'));
            item->m_model.append(screen->serialNumber());
        }
    } else if (!screen->serialNumber().isEmpty()) {
        item->m_model = screen->serialNumber();
    }
    if (item->m_model.isEmpty())
        item->m_model = QStringLiteral("Unknown");

    item->m_name = screen->name();
    item->m_description = screen->manufacturer() + QStringLiteral(" ") + screen->model();
    item->m_position = screen->availableGeometry().topLeft();
    item->m_physicalSize = screen->physicalSize();
    item->m_scaleFactor = qFloor(screen->devicePixelRatio());

    QPlatformScreen::SubpixelAntialiasingType subpixel = screen->handle()->subpixelAntialiasingTypeHint();
    switch (subpixel) {
    case QPlatformScreen::Subpixel_None:
        item->m_subpixel = WaylandOutput::SubpixelNone;
        break;
    case QPlatformScreen::Subpixel_RGB:
        item->m_subpixel = WaylandOutput::SubpixelHorizontalRgb;
        break;
    case QPlatformScreen::Subpixel_BGR:
        item->m_subpixel = WaylandOutput::SubpixelHorizontalBgr;
        break;
    case QPlatformScreen::Subpixel_VRGB:
        item->m_subpixel = WaylandOutput::SubpixelVerticalRgb;
        break;
    case QPlatformScreen::Subpixel_VBGR:
        item->m_subpixel = WaylandOutput::SubpixelVerticalBgr;
        break;
    }

    switch (screen->orientation()) {
    case Qt::PortraitOrientation:
        item->m_transform = WaylandOutput::Transform90;
        break;
    case Qt::InvertedLandscapeOrientation:
        item->m_transform = WaylandOutput::Transform180;
        break;
    case Qt::InvertedPortraitOrientation:
        item->m_transform = WaylandOutput::Transform270;
        break;
    default:
        break;
    }

    if (screen->handle()->modes().count() == 0) {
        ScreenMode *mode = new ScreenMode(item);
        mode->m_resolution = screen->geometry().size();
        mode->m_refreshRate = qFloor(screen->refreshRate() * 1000);
        item->m_modes.append(mode);
        item->m_currentMode = mode;
        item->m_preferredMode = mode;
    } else {
        int modeIndex = 0;
        const auto modes = screen->handle()->modes();
        for (const auto &platformMode : modes) {
            const QSize size = platformMode.size;
            const int refresh = qFloor(platformMode.refreshRate * 1000);

            // Don't add the same modes twice
            auto result = std::find_if(item->m_modes.begin(), item->m_modes.end(), [size, refresh](ScreenMode *mode) {
                return mode->resolution() == size && mode->refreshRate() == refresh;
            });
            if (result != item->m_modes.end())
                continue;

            ScreenMode *mode = new ScreenMode(item);
            mode->m_resolution = size;
            mode->m_refreshRate = refresh;
            item->m_modes.append(mode);

            if (screen->handle()->currentMode() == modeIndex)
                item->m_currentMode = mode;
            if (screen->handle()->preferredMode() == modeIndex)
                item->m_preferredMode = mode;

            modeIndex++;
        }
    }

    connect(screen, &QScreen::availableGeometryChanged, this, [this, item](const QRect &geometry) {
        int row = m_items.indexOf(item);

        if (item->m_position != geometry.topLeft()) {
            item->m_position = geometry.topLeft();
            Q_EMIT item->positionChanged();
        }

        for (auto *mode : qAsConst(item->m_modes)) {
            const QSize size = geometry.size();
            const int refreshRate = qFloor(item->screen()->refreshRate() * 1000);
            if (mode->resolution() == size && mode->refreshRate() == refreshRate) {
                if (item->m_currentMode != mode) {
                    item->m_currentMode = mode;
                    Q_EMIT item->currentModeChanged(size, refreshRate);
                }
                break;
            }
        }

        Q_EMIT dataChanged(index(row, 0), index(row, 0));
    });
    connect(screen, &QScreen::refreshRateChanged, this, [this, item](qreal rr) {
        int row = m_items.indexOf(item);

        for (auto *mode : qAsConst(item->m_modes)) {
            const QSize size = item->screen()->availableGeometry().size();
            const int refreshRate = qFloor(rr * 1000);
            if (mode->resolution() == size && mode->refreshRate() == refreshRate) {
                if (item->m_currentMode != mode) {
                    item->m_currentMode = mode;
                    Q_EMIT item->currentModeChanged(size, refreshRate);
                }
                break;
            }
        }

        Q_EMIT dataChanged(index(row, 0), index(row, 0));
    });
    connect(screen, &QScreen::physicalSizeChanged, this, [this, item](const QSizeF &size) {
        int row = m_items.indexOf(item);
        item->m_physicalSize = size;
        Q_EMIT item->physicalSizeChanged();
        Q_EMIT dataChanged(index(row, 0), index(row, 0));
    });
    connect(screen, &QScreen::primaryOrientationChanged, this, [this, item](Qt::ScreenOrientation orientation) {
        int row = m_items.indexOf(item);
        switch (orientation) {
        case Qt::PortraitOrientation:
            item->m_transform = WaylandOutput::Transform90;
            break;
        case Qt::InvertedLandscapeOrientation:
            item->m_transform = WaylandOutput::Transform180;
            break;
        case Qt::InvertedPortraitOrientation:
            item->m_transform = WaylandOutput::Transform270;
            break;
        default:
            break;
        }
        Q_EMIT item->transformChanged();
        Q_EMIT dataChanged(index(row, 0), index(row, 0));
    });

    m_items.append(item);

    endInsertRows();
}
