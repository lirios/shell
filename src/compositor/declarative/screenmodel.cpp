/****************************************************************************
 * This file is part of Liri.
 *
 * Copyright (C) 2018 Pier Luigi Fiorini <pierluigi.fiorini@gmail.com>
 *
 * $BEGIN_LICENSE:GPL3+$
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 *
 * $END_LICENSE$
 ***************************************************************************/

#include <QCryptographicHash>
#include <QFile>
#include <QGuiApplication>
#include <QJsonArray>
#include <QJsonDocument>
#include <QJsonObject>
#include <QtMath>
#include <qpa/qplatformscreen.h>

#include <LiriPlatformHeaders/lirieglfsfunctions.h>

#include "declarative/screenmodel.h"
#include "logging_p.h"

static ScreenItem::PowerState convertPowerState(Liri::Platform::EglFSFunctions::PowerState powerState)
{
    switch (powerState) {
    case Liri::Platform::EglFSFunctions::PowerStateOn:
        return ScreenItem::PowerStateOn;
    case Liri::Platform::EglFSFunctions::PowerStateStandby:
        return ScreenItem::PowerStateStandby;
    case Liri::Platform::EglFSFunctions::PowerStateSuspend:
        return ScreenItem::PowerStateSuspend;
    case Liri::Platform::EglFSFunctions::PowerStateOff:
        return ScreenItem::PowerStateOff;
    }
    Q_UNREACHABLE();
}

static Liri::Platform::EglFSFunctions::PowerState convertPowerState(ScreenItem::PowerState powerState)
{
    switch (powerState) {
    case ScreenItem::PowerStateOn:
        return Liri::Platform::EglFSFunctions::PowerStateOn;
    case ScreenItem::PowerStateStandby:
        return Liri::Platform::EglFSFunctions::PowerStateStandby;
    case ScreenItem::PowerStateSuspend:
        return Liri::Platform::EglFSFunctions::PowerStateSuspend;
    case ScreenItem::PowerStateOff:
        return Liri::Platform::EglFSFunctions::PowerStateOff;
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

bool ScreenItem::isPrimary() const
{
    return m_primary;
}

QString ScreenItem::uuid() const
{
    return m_uuid;
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

int ScreenItem::x() const
{
    return m_geometry.x();
}

int ScreenItem::y() const
{
    return m_geometry.y();
}

int ScreenItem::width() const
{
    return m_geometry.width();
}

int ScreenItem::height() const
{
    return m_geometry.height();
}

QPoint ScreenItem::position() const
{
    return m_geometry.topLeft();
}

void ScreenItem::setPosition(const QPoint &pos)
{
    if (!m_screen) {
        qCWarning(lcShell) << "ScreenItem cannot set position if the screen property is not set";
        return;
    }

    if (m_geometry.topLeft() == pos)
        return;

    Liri::Platform::EglFSFunctions::setScreenPosition(m_screen, pos);
    m_geometry.setTopLeft(pos);
    Q_EMIT geometryChanged();
}

QRect ScreenItem::geometry() const
{
    return m_geometry;
}

QSizeF ScreenItem::physicalSize() const
{
    return m_physicalSize;
}

int ScreenItem::scaleFactor() const
{
    return m_scaleFactor;
}

void ScreenItem::setScaleFactor(int factor)
{
    if (!m_screen) {
        qCWarning(lcShell) << "ScreenItem cannot set scale factor if the screen property is not set";
        return;
    }

    if (m_scaleFactor == factor)
        return;

    Liri::Platform::EglFSFunctions::setScreenScaleFactor(m_screen, static_cast<qreal>(factor));
    m_scaleFactor = factor;
    Q_EMIT scaleFactorChanged();
}

QWaylandOutput::Subpixel ScreenItem::subpixel() const
{
    return m_subpixel;
}

QWaylandOutput::Transform ScreenItem::transform() const
{
    return m_transform;
}

QQmlListProperty<ScreenMode> ScreenItem::modes()
{
    auto countFunc = [](QQmlListProperty<ScreenMode> *prop) {
        return static_cast<ScreenItem *>(prop->object)->m_modes.count();
    };
    auto atFunc = [](QQmlListProperty<ScreenMode> *prop, int index) {
        return static_cast<ScreenItem *>(prop->object)->m_modes.at(index);
    };
    return QQmlListProperty<ScreenMode>(this, this, countFunc, atFunc);
}

int ScreenItem::currentModeIndex() const
{
    return m_currentMode;
}

void ScreenItem::setCurrentModeIndex(int modeIndex)
{
    if (!m_screen) {
        qCWarning(lcShell) << "ScreenItem cannot set current mode if the screen property is not set";
        return;
    }

    Liri::Platform::EglFSFunctions::setScreenMode(m_screen, modeIndex);
    m_currentMode = modeIndex;
    Q_EMIT currentModeIndexChanged();
}

int ScreenItem::preferredModeIndex() const
{
    return m_preferredMode;
}

ScreenItem::PowerState ScreenItem::powerState() const
{
    if (!m_screen) {
        qCWarning(lcShell) << "ScreenItem cannot get power state if the screen property is not set";
        return ScreenItem::PowerStateOn;
    }

    return convertPowerState(Liri::Platform::EglFSFunctions::getPowerState(m_screen));
}

void ScreenItem::setPowerState(ScreenItem::PowerState state)
{
    if (!m_screen) {
        qCWarning(lcShell) << "ScreenItem cannot set power state if the screen property is not set";
        return;
    }

    auto oldPowerState = Liri::Platform::EglFSFunctions::getPowerState(m_screen);
    auto newPowerState = convertPowerState(state);

    if (oldPowerState == newPowerState)
        return;

    Liri::Platform::EglFSFunctions::setPowerState(m_screen, newPowerState);

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
        qWarning("Screen configuration cannot be changed after initialization");
        return;
    }

    m_fileName = fileName;
    Q_EMIT fileNameChanged();
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
    roles.insert(PrimaryRole, QByteArrayLiteral("primary"));
    roles.insert(ManufacturerRole, QByteArrayLiteral("manufacturer"));
    roles.insert(ModelRole, QByteArrayLiteral("model"));
    roles.insert(NameRole, QByteArrayLiteral("name"));
    roles.insert(XRole, QByteArrayLiteral("x"));
    roles.insert(YRole, QByteArrayLiteral("y"));
    roles.insert(WidthRole, QByteArrayLiteral("width"));
    roles.insert(HeightRole, QByteArrayLiteral("height"));
    roles.insert(PhysicalSizeRole, QByteArrayLiteral("physicalSize"));
    roles.insert(ScaleFactorRole, QByteArrayLiteral("scaleFactor"));
    roles.insert(SubpixelRole, QByteArrayLiteral("subpixel"));
    roles.insert(TransformRole, QByteArrayLiteral("transform"));
    roles.insert(ModesRole, QByteArrayLiteral("modes"));
    roles.insert(CurrentModeIndex, QByteArrayLiteral("currentModeIndex"));
    roles.insert(PreferredModeIndex, QByteArrayLiteral("preferredModeIndex"));
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
    case PrimaryRole:
        return item->isPrimary();
    case UuidRole:
        return item->uuid();
    case ManufacturerRole:
        return item->manufacturer();
    case ModelRole:
        return item->model();
    case NameRole:
        return item->name();
    case XRole:
        return item->geometry().topLeft().rx();
    case YRole:
        return item->geometry().topLeft().ry();
    case WidthRole:
        return item->geometry().width();
    case HeightRole:
        return item->geometry().height();
    case PhysicalSizeRole:
        return item->physicalSize();
    case ScaleFactorRole:
        return item->scaleFactor();
    case SubpixelRole:
        return item->subpixel();
    case TransformRole:
        return item->transform();
    case ModesRole:
        return QVariant::fromValue(item->modes());
    case CurrentModeIndex:
        return item->currentModeIndex();
    case PreferredModeIndex:
        return item->preferredModeIndex();
    }

    return QVariant();
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
        for (const QScreen *screen : qGuiApp->screens())
            handleScreenAdded(const_cast<QScreen *>(screen));

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
        connect(qGuiApp, &QGuiApplication::primaryScreenChanged, this, [this](QScreen *screen) {
            int row = 0;
            for (auto screenItem : m_items) {
                if (screenItem->screen()) {
                    screenItem->m_primary = screenItem->screen() == screen;
                    Q_EMIT screenItem->primaryChanged();
                    Q_EMIT dataChanged(index(row, 0), index(row, 0));
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

    bool primarySet = false;

    const QJsonObject object = doc.object();
    const QJsonArray outputs = object.value(QStringLiteral("outputs")).toArray();

    for (int i = 0; i < outputs.size(); i++) {
        const QVariantMap outputSettings = outputs.at(i).toObject().toVariantMap();
        qCDebug(lcShell) << "Output settings:" << outputSettings;

        QString name = outputSettings.value(QStringLiteral("name")).toString();
        qCDebug(lcShell) << "Output name:" << name;

        bool primary = outputSettings.value(QStringLiteral("primary")).toBool();
        qCDebug(lcShell) << "Output primary:" << primary;

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
        qCDebug(lcShell) << "Physical size millimiters:" << size;

        Qt::ScreenOrientation orientation =
                static_cast<Qt::ScreenOrientation>(outputSettings.value(QStringLiteral("orientation")).toInt());
        qCDebug(lcShell) << "Output orientation:" << orientation;

        QCryptographicHash hash(QCryptographicHash::Md5);
        hash.addData(QByteArrayLiteral("Liri"));
        hash.addData(name.toUtf8());

        beginInsertRows(QModelIndex(), m_items.count(), m_items.count());

        ScreenItem *item = new ScreenItem(this);
        item->m_uuid = hash.result().toHex().left(10);
        item->m_primary = primary && !primarySet;
        item->m_manufacturer = QStringLiteral("Liri");
        item->m_model = name;
        if (physicalSize.isValid())
            item->m_physicalSize = physicalSize;
        item->m_geometry = QRect(pos, size);
        item->m_scaleFactor = scale;

        if (primary)
            primarySet = true;

        switch (orientation) {
        case Qt::PortraitOrientation:
            item->m_transform = QWaylandOutput::Transform90;
            break;
        case Qt::InvertedLandscapeOrientation:
            item->m_transform = QWaylandOutput::Transform180;
            break;
        case Qt::InvertedPortraitOrientation:
            item->m_transform = QWaylandOutput::Transform270;
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

        Q_EMIT item->primaryChanged();
    }
}

void ScreenModel::handleScreenAdded(QScreen *screen)
{
    beginInsertRows(QModelIndex(), m_items.count(), m_items.count());

    ScreenItem *item = new ScreenItem(this);
    item->m_screen = screen;
    item->m_primary = screen == qGuiApp->primaryScreen();

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
    item->m_geometry = screen->availableGeometry();
    item->m_physicalSize = screen->physicalSize();
    item->m_scaleFactor = qFloor(screen->devicePixelRatio());

    QCryptographicHash hash(QCryptographicHash::Md5);
    hash.addData(item->m_manufacturer.toUtf8());
    hash.addData(item->m_model.toUtf8());
    item->m_uuid = hash.result().toHex().left(10);

    QPlatformScreen::SubpixelAntialiasingType subpixel = screen->handle()->subpixelAntialiasingTypeHint();
    switch (subpixel) {
    case QPlatformScreen::Subpixel_None:
        item->m_subpixel = QWaylandOutput::SubpixelNone;
        break;
    case QPlatformScreen::Subpixel_RGB:
        item->m_subpixel = QWaylandOutput::SubpixelHorizontalRgb;
        break;
    case QPlatformScreen::Subpixel_BGR:
        item->m_subpixel = QWaylandOutput::SubpixelHorizontalBgr;
        break;
    case QPlatformScreen::Subpixel_VRGB:
        item->m_subpixel = QWaylandOutput::SubpixelVerticalRgb;
        break;
    case QPlatformScreen::Subpixel_VBGR:
        item->m_subpixel = QWaylandOutput::SubpixelVerticalBgr;
        break;
    }

    switch (screen->orientation()) {
    case Qt::PortraitOrientation:
        item->m_transform = QWaylandOutput::Transform90;
        break;
    case Qt::InvertedLandscapeOrientation:
        item->m_transform = QWaylandOutput::Transform180;
        break;
    case Qt::InvertedPortraitOrientation:
        item->m_transform = QWaylandOutput::Transform270;
        break;
    default:
        break;
    }

    if (screen->handle()->modes().count() == 0) {
        ScreenMode *mode = new ScreenMode(item);
        mode->m_resolution = screen->geometry().size();
        mode->m_refreshRate = int(screen->refreshRate() * 1000);
        item->m_modes.append(mode);
    } else {
        for (auto platformMode : screen->handle()->modes()) {
            ScreenMode *mode = new ScreenMode(item);
            mode->m_resolution = platformMode.size;
            mode->m_refreshRate = int(platformMode.refreshRate * 1000);
            item->m_modes.append(mode);
        }
    }
    item->m_currentMode = screen->handle()->currentMode();
    item->m_preferredMode = screen->handle()->preferredMode();

    connect(screen, &QScreen::availableGeometryChanged, this, [this, item](const QRect &geometry) {
        int row = m_items.indexOf(item);

        item->m_geometry = geometry;
        Q_EMIT item->geometryChanged();

        int modeIndex = 0;
        for (auto mode : item->m_modes) {
            const QSize size = geometry.size();
            const int refreshRate = int(item->screen()->refreshRate() * 1000);
            if (mode->resolution() == size && mode->refreshRate() == refreshRate) {
                item->m_currentMode = modeIndex;
                Q_EMIT item->currentModeIndexChanged();
                break;
            }
            modeIndex++;
        }

        Q_EMIT dataChanged(index(row, 0), index(row, 0));
    });
    connect(screen, &QScreen::refreshRateChanged, this, [this, item](qreal rr) {
        int row = m_items.indexOf(item);

        int modeIndex = 0;
        for (auto mode : item->m_modes) {
            const QSize size = item->screen()->availableGeometry().size();
            const int refreshRate = int(rr * 1000);
            if (mode->resolution() == size && mode->refreshRate() == refreshRate) {
                item->m_currentMode = modeIndex;
                Q_EMIT item->currentModeIndexChanged();
                break;
            }
            modeIndex++;
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
            item->m_transform = QWaylandOutput::Transform90;
            break;
        case Qt::InvertedLandscapeOrientation:
            item->m_transform = QWaylandOutput::Transform180;
            break;
        case Qt::InvertedPortraitOrientation:
            item->m_transform = QWaylandOutput::Transform270;
            break;
        default:
            break;
        }
        Q_EMIT item->transformChanged();
        Q_EMIT dataChanged(index(row, 0), index(row, 0));
    });

    m_items.append(item);

    endInsertRows();

    Q_EMIT item->primaryChanged();
}
