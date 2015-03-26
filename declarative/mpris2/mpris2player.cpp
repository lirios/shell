/****************************************************************************
* This file is part of Hawaii.
 *
 * Copyright (C) 2015 Pier Luigi Fiorini <pierluigi.fiorini@gmail.com>
 *
 * Author(s):
 *    Pier Luigi Fiorini
 *
 * $BEGIN_LICENSE:LGPL2.1+$
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License as published by
 * the Free Software Foundation, either version 2.1 of the License, or
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

/*
 * Code to update data from properties is partially:
 * Copyright 2012 Alex Merry <alex.merry@kdemail.net>
 */

#include <QtCore/QUrl>
#include <QtCore/QSettings>
#include <QtDBus/QDBusArgument>
#include <QtDBus/QDBusMetaType>

#include "mpris2player.h"
#include "mprisadaptor.h"
#include "mprisplayeradaptor.h"
#include "dbuspropertiesadaptor.h"

const QString objectPath("/org/mpris/MediaPlayer2");
const QString playerInterface("org.mpris.MediaPlayer2.Player");
const QString mprisInterface("org.mpris.MediaPlayer2");

Q_LOGGING_CATEGORY(MPRIS2_PLAYER, "hawaii.qml.mpris2.player")

static QVariant::Type propertyType(const QString &name)
{
    if (name == QStringLiteral("Identity"))
        return QVariant::String;
    if (name == QStringLiteral("DesktopEntry"))
        return QVariant::String;
    if (name == QStringLiteral("SupportedUriSchemes"))
        return QVariant::StringList;
    if (name == QStringLiteral("SupportedMimeTypes"))
        return QVariant::StringList;
    if (name == QStringLiteral("Fullscreen"))
        return QVariant::Bool;
    if (name == QStringLiteral("PlaybackStatus"))
        return QVariant::String;
    if (name == QStringLiteral("LoopStatus"))
        return QVariant::String;
    if (name == QStringLiteral("Shuffle"))
        return QVariant::Bool;
    if (name == QStringLiteral("Rate"))
        return QVariant::Double;
    if (name == QStringLiteral("MinimumRate"))
        return QVariant::Double;
    if (name == QStringLiteral("MaximumRate"))
        return QVariant::Double;
    if (name == QStringLiteral("Volume"))
        return QVariant::Double;
    if (name == QStringLiteral("Position"))
        return QVariant::LongLong;
    if (name == QStringLiteral("Metadata"))
        return QVariant::Map;
    if (name == QStringLiteral("CanControl"))
        return QVariant::Bool;
    if (name == QStringLiteral("CanSeek"))
        return QVariant::Bool;
    if (name == QStringLiteral("CanGoNext"))
        return QVariant::Bool;
    if (name == QStringLiteral("CanGoPrevious"))
        return QVariant::Bool;
    if (name == QStringLiteral("CanRaise"))
        return QVariant::Bool;
    return QVariant::Invalid;
}

static Mpris2Player::Capability capabilityFromName(const QString &name)
{
    if (name == QStringLiteral("CanQuit"))
        return Mpris2Player::CanQuit;
    if (name == QStringLiteral("CanRaise"))
        return Mpris2Player::CanQuit;
    if (name == QStringLiteral("CanSetFullscreen"))
        return Mpris2Player::CanSetFullscreen;
    if (name == QStringLiteral("CanControl"))
        return Mpris2Player::CanControl;
    if (name == QStringLiteral("CanPlay"))
        return Mpris2Player::CanPlay;
    if (name == QStringLiteral("CanPause"))
        return Mpris2Player::CanPause;
    if (name == QStringLiteral("CanSeek"))
        return Mpris2Player::CanSeek;
    if (name == QStringLiteral("CanGoNext"))
        return Mpris2Player::CanGoNext;
    if (name == QStringLiteral("CanGoPrevious"))
        return Mpris2Player::CanGoPrevious;
    return Mpris2Player::NoCapabilities;
}

static bool decodeUri(QVariantMap &map, const QString &entry) {
    if (map.contains(entry)) {
        QString urlString = map.value(entry).toString();
        QUrl url = QUrl::fromEncoded(urlString.toUtf8());
        if (!url.isValid()) {
            // try to be lenient
            url = QUrl(urlString);
        }
        if (url.isValid()) {
            map.insert(entry, QVariant(url));
            return true;
        } else {
            map.remove(entry);
            return false;
        }
    }

    // count it as a success if it doesn't exist
    return true;
}

Mpris2Player::Mpris2Player(const QString &service, QObject *parent)
    : QObject(parent)
    , m_serviceName(service)
    , m_fetchesPending(0)
    , m_capabilities(NoCapabilities)
    , m_metadata(new QQmlPropertyMap(this))
    , m_status(QStringLiteral("Stopped"))
    , m_fullScreen(false)
    , m_position(0)
    , m_rate(0)
    , m_minimumRate(0)
    , m_maximumRate(0)
    , m_volume(0)
{
    // Create D-Bus adaptors
    m_propsInterface = new OrgFreedesktopDBusPropertiesInterface(service, objectPath,
                                                                 QDBusConnection::sessionBus(),
                                                                 this);
    m_interface = new OrgMprisMediaPlayer2Interface(service, objectPath,
                                                    QDBusConnection::sessionBus(),
                                                    this);
    m_playerInterface = new OrgMprisMediaPlayer2PlayerInterface(service, objectPath,
                                                                QDBusConnection::sessionBus(),
                                                                this);

    // Receive properties changes
    connect(m_propsInterface, &OrgFreedesktopDBusPropertiesInterface::PropertiesChanged,
            [=](const QString &interface, const QVariantMap &changedProperties, const QStringList &invalidatedProperties) {
        Q_UNUSED(interface);

        updateFromMap(changedProperties);
        if (!invalidatedProperties.isEmpty())
            retrieveData();
    });
    connect(m_playerInterface, &OrgMprisMediaPlayer2PlayerInterface::Seeked, [=](qlonglong offset) {
        m_position = offset;
        Q_EMIT positionChanged();
    });

    // Retrieve data
    retrieveData();
}

QString Mpris2Player::serviceName() const
{
    return m_serviceName;
}

QString Mpris2Player::identity() const
{
    return m_identity;
}

QString Mpris2Player::iconName() const
{
    return m_iconName;
}

Mpris2Player::Capabilities Mpris2Player::capabilities() const
{
    return m_capabilities;
}

QQmlPropertyMap *Mpris2Player::metadata() const
{
    return m_metadata;
}

QString Mpris2Player::status() const
{
    return m_status;
}

bool Mpris2Player::isFullScreen() const
{
    return m_fullScreen;
}

qlonglong Mpris2Player::position() const
{
    return m_position;
}

qreal Mpris2Player::rate() const
{
    return m_rate;
}

qreal Mpris2Player::minimumRate() const
{
    return m_minimumRate;
}

qreal Mpris2Player::maximumRate() const
{
    return m_maximumRate;
}

void Mpris2Player::raise()
{
    m_interface->Raise();
}

void Mpris2Player::quit()
{
    m_interface->Quit();
}

void Mpris2Player::previous()
{
    m_playerInterface->Previous();
}

void Mpris2Player::next()
{
    m_playerInterface->Next();
}

void Mpris2Player::play()
{
    m_playerInterface->Play();
}

void Mpris2Player::pause()
{
    m_playerInterface->Pause();
}

void Mpris2Player::playPause()
{
    m_playerInterface->PlayPause();
}

void Mpris2Player::stop()
{
    m_playerInterface->Stop();
}

void Mpris2Player::seek(qlonglong offset)
{
    m_playerInterface->Seek(offset);
}

void Mpris2Player::setPosition(const QString &trackId, qlonglong position)
{
    m_playerInterface->SetPosition(QDBusObjectPath(trackId), position);
}

void Mpris2Player::openUrl(const QUrl &url)
{
    m_playerInterface->OpenUri(url.toString());
}

void Mpris2Player::retrieveData()
{
    // despite these calls being async, we should never update values in the
    // wrong order (eg: a stale GetAll response overwriting a more recent value
    // from a PropertiesChanged signal) due to D-Bus message ordering guarantees.

    QDBusPendingCall async = m_propsInterface->GetAll(OrgMprisMediaPlayer2Interface::staticInterfaceName());
    QDBusPendingCallWatcher *watcher = new QDBusPendingCallWatcher(async, this);
    connect(watcher, SIGNAL(finished(QDBusPendingCallWatcher*)),
            this, SLOT(propertiesFinished(QDBusPendingCallWatcher*)));
    ++m_fetchesPending;

    async = m_propsInterface->GetAll(OrgMprisMediaPlayer2PlayerInterface::staticInterfaceName());
    watcher = new QDBusPendingCallWatcher(async, this);
    connect(watcher, SIGNAL(finished(QDBusPendingCallWatcher*)),
            this, SLOT(propertiesFinished(QDBusPendingCallWatcher*)));
    ++m_fetchesPending;
}

void Mpris2Player::setCapabilities(Capabilities cap)
{
    if (m_capabilities == cap)
        return;

    m_capabilities = cap;
    Q_EMIT capabilitiesChanged();
}

void Mpris2Player::setMetadata(const Metadata &map)
{
    m_metadata->deleteLater();
    m_metadata = new QQmlPropertyMap(this);
    for (const QString &key: map.keys())
        m_metadata->insert(key, map.value(key));
    Q_EMIT metadataChanged();
}

void Mpris2Player::calculatePosition()
{
    qint64 diff = m_lastPosUpdate.msecsTo(QDateTime::currentDateTimeUtc()) * 1000;
    m_position = m_position + static_cast<qint64>(diff - m_rate);
    Q_EMIT positionChanged();
}

void Mpris2Player::propertiesFinished(QDBusPendingCallWatcher *watcher)
{
    QDBusPendingReply<QVariantMap> propsReply = *watcher;
    watcher->deleteLater();

    if (m_fetchesPending < 1)
        return;

    if (propsReply.isError()) {
        qCWarning(MPRIS2_PLAYER) << m_serviceName << "does not implement"
                                 << OrgFreedesktopDBusPropertiesInterface::staticInterfaceName()
                                 << "correctly";
        qCDebug(MPRIS2_PLAYER) << "Error message was" << propsReply.error().name() << propsReply.error().message();
        m_fetchesPending = 0;
        Q_EMIT initialFetchFailed();
        return;
    }

    updateFromMap(propsReply.value());

    --m_fetchesPending;
    if (m_fetchesPending == 0)
        Q_EMIT initialFetchFinished();
}

void Mpris2Player::updateFromMap(const QVariantMap &map)
{
    QMap<QString, QVariant>::const_iterator i = map.constBegin();
    while (i != map.constEnd()) {
        // Properties
        QVariant::Type type = propertyType(i.key());
        if (type != QVariant::Invalid)
            copyProperty(i.key(), i.value(), type);

        // Capabilities
        Capability capability = capabilityFromName(i.key());
        if (capability != NoCapabilities) {
            if (i.value().type() == QVariant::Bool) {
                if (i.value().toBool())
                    m_capabilities |= capability;
                else
                    m_capabilities &= ~capability;
            } else {
                const char *gotTypeCh = QDBusMetaType::typeToSignature(i.value().userType());
                QString gotType = gotTypeCh ? QString::fromUtf8(gotTypeCh) : QStringLiteral("<unknown>");

                qCWarning(MPRIS2_PLAYER) << m_serviceName << "exports" << i.key()
                                         << "as D-Bus type" << gotType
                                         << "but it should be D-Bus type \"b\"";
            }
        }

        // Fake the CanStop capability
        if (capability == CanControl || i.key() == QStringLiteral("PlaybackStatus")) {
            if ((m_capabilities & CanControl) && i.value().toString() != QStringLiteral("Stopped")) {
                qCDebug(MPRIS2_PLAYER) << "Enabling stop capability";
                m_capabilities |= CanStop;
            } else {
                qCDebug(MPRIS2_PLAYER) << "Disabling stop capability";
                m_capabilities &= ~CanStop;
            }
        }

        ++i;
    }
}

void Mpris2Player::copyProperty(const QString &name, const QVariant &value,
                                const QVariant::Type &expectedType)
{
    QVariant tmp = value;

    // We protect our users from bogus values
    if (tmp.userType() == qMetaTypeId<QDBusArgument>()) {
        if (expectedType == QVariant::Map) {
            QDBusArgument arg = tmp.value<QDBusArgument>();
            if (arg.currentType() != QDBusArgument::MapType) {
                qCWarning(MPRIS2_PLAYER) << m_serviceName << "exports" << name
                                         << "with the wrong type; it should be D-Bus type \"a{sv}\"";
                return;
            }
            QVariantMap map;
            arg >> map;
            if (name == QStringLiteral("Metadata")) {
                if (!decodeUri(map, QStringLiteral("mpris:artUrl"))) {
                    qCWarning(MPRIS2_PLAYER) << m_serviceName
                                             << "has an invalid URL for the mpris:artUrl entry of the \"Metadata\" property";
                }
                if (!decodeUri(map, QStringLiteral("xesam:url"))) {
                    qCWarning(MPRIS2_PLAYER) << m_serviceName
                                             << "has an invalid URL for the xesam:url entry of the \"Metadata\" property";
                }
            }
            tmp = QVariant(map);
        }
    }
    if (tmp.type() != expectedType) {
        const char * gotTypeCh = QDBusMetaType::typeToSignature(tmp.userType());
        QString gotType = gotTypeCh ? QString::fromUtf8(gotTypeCh) : "<unknown>";
        const char * expTypeCh = QDBusMetaType::typeToSignature(expectedType);
        QString expType = expTypeCh ? QString::fromUtf8(expTypeCh) : "<unknown>";

        qCWarning(MPRIS2_PLAYER) << m_serviceName << "exports" << name
                                 << "as D-Bus type" << gotType
                                 << "but it should be D-Bus type" << expType;
    }

    if (!tmp.convert(expectedType))
        return;

    if (name == QStringLiteral("Metadata")) {
        // Reset position if the track has changed
        const QString oldTrackId = m_metadata->value(QStringLiteral("mpris:trackid")).toString();
        const QString newTrackId = tmp.toMap().value(QStringLiteral("mpris:trackid")).toString();
        if (oldTrackId != newTrackId) {
            m_position = 0;
            m_lastPosUpdate = QDateTime::currentDateTimeUtc();
            Q_EMIT positionChanged();
        }

        // Set new metadata and remove invalid length
        QMap<QString, QVariant> map = tmp.toMap();
        if (map.value(QStringLiteral("mpris:length")).toLongLong() <= 0)
            map.remove(QStringLiteral("mpris:length"));
        setMetadata(map);
    } else if (name == QStringLiteral("Position")) {
        m_lastPosUpdate = QDateTime::currentDateTimeUtc();
    } else if (name == QStringLiteral("Rate")) {
        if (m_status == QStringLiteral("Playing"))
            calculatePosition();

        if (m_rate != value.toDouble()) {
            m_rate = value.toDouble();
            Q_EMIT rateChanged();
        }
    } else if (name == QStringLiteral("MinimumRate")) {
        if (m_minimumRate != value.toDouble()) {
            m_minimumRate = value.toDouble();
            Q_EMIT minimumRateChanged();
        }
    } else if (name == QStringLiteral("MaximumRate")) {
        if (m_maximumRate != value.toDouble()) {
            m_maximumRate = value.toDouble();
            Q_EMIT maximumRateChanged();
        }
    } else if (name == QStringLiteral("Volume")) {
        if (m_volume != value.toDouble()) {
            m_volume = value.toDouble();
            Q_EMIT volumeChanged();
        }
    } else if (name == QStringLiteral("PlaybackStatus")) {
        // Reset rate if it's not playing
        if (value.toString() != QStringLiteral("Playing")) {
            m_rate = 0.0;
            Q_EMIT rateChanged();
        }

        // Set position to 0 if stopped
        if (value.toString() == QStringLiteral("Stopped")) {
            m_position = 0;
            m_lastPosUpdate = QDateTime::currentDateTimeUtc();
            Q_EMIT positionChanged();
        }

        // Set status
        if (m_status != value.toString()) {
            m_status = value.toString();
            Q_EMIT statusChanged();
        }
    } else if (name == QStringLiteral("Identity")) {
        if (m_identity != value.toString()) {
            m_identity = value.toString();
            Q_EMIT identityChanged();
        }
    } else if (name == QStringLiteral("DesktopEntry")) {
        QSettings desktopFile(value.toString() + QStringLiteral(".desktop"), QSettings::IniFormat);
        desktopFile.setIniCodec("UTF-8");
        desktopFile.beginGroup(QStringLiteral("Desktop Entry"));
        QString iconName = desktopFile.value(QStringLiteral("Icon")).toString();
        if (!iconName.isEmpty()) {
            m_iconName = iconName;
            Q_EMIT iconNameChanged();
        }
    } else if (name == QStringLiteral("Fullscreen")) {
        if (m_fullScreen != value.toBool()) {
            m_fullScreen = value.toBool();
            Q_EMIT fullScreenChanged();
        }
    } else {
        qCWarning(MPRIS2_PLAYER) << "Unhandled property:" << name;
    }
}

#include "moc_mpris2player.cpp"
