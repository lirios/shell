// SPDX-FileCopyrightText: 2018 Pier Luigi Fiorini <pierluigi.fiorini@gmail.com>
//
// SPDX-License-Identifier: LGPL-3.0-or-later

#ifndef LIRI_MPRIS2PLAYER_H
#define LIRI_MPRIS2PLAYER_H

#include <QtCore/QObject>
#include <QtCore/QDateTime>
#include <QtCore/QLoggingCategory>
#include <QtDBus/QDBusMessage>
#include <QtQml/QQmlPropertyMap>

class QDBusPendingCallWatcher;
class OrgFreedesktopDBusPropertiesInterface;
class OrgMprisMediaPlayer2Interface;
class OrgMprisMediaPlayer2PlayerInterface;

Q_DECLARE_LOGGING_CATEGORY(MPRIS2_PLAYER)

typedef QMap<QString, QVariant> Metadata;

class Mpris2Player : public QObject
{
    Q_OBJECT
    Q_PROPERTY(QString identity READ identity NOTIFY identityChanged)
    Q_PROPERTY(QString iconName READ iconName NOTIFY iconNameChanged)
    Q_PROPERTY(Capabilities capabilities READ capabilities WRITE setCapabilities NOTIFY capabilitiesChanged)
    Q_PROPERTY(QQmlPropertyMap *metadata READ metadata NOTIFY metadataChanged)
    Q_PROPERTY(qlonglong position READ position NOTIFY positionChanged)
    Q_PROPERTY(QString status READ status NOTIFY statusChanged)
    Q_PROPERTY(bool fullScreen READ isFullScreen NOTIFY fullScreenChanged)
    Q_PROPERTY(qreal rate READ rate NOTIFY rateChanged)
    Q_PROPERTY(qreal minimumRate READ minimumRate NOTIFY minimumRateChanged)
    Q_PROPERTY(qreal maximumRate READ maximumRate NOTIFY maximumRateChanged)
public:
    enum Capability {
        NoCapabilities = 0,
        CanQuit,
        CanRaise,
        CanSetFullscreen,
        CanControl,
        CanPlay,
        CanPause,
        CanStop,
        CanSeek,
        CanGoNext,
        CanGoPrevious,
    };
    Q_ENUM(Capability)
    Q_DECLARE_FLAGS(Capabilities, Capability)

    Mpris2Player(const QString &service, QObject *parent = 0);

	QString serviceName() const;

    QString identity() const;
    QString iconName() const;

    Capabilities capabilities() const;

    QQmlPropertyMap *metadata() const;

    QString status() const;

    bool isFullScreen() const;

    qlonglong position() const;

    qreal rate() const;
    qreal minimumRate() const;
    qreal maximumRate() const;

    qreal volume() const;

Q_SIGNALS:
    void identityChanged();
    void iconNameChanged();
    void capabilitiesChanged();
    void metadataChanged();
    void statusChanged();
    void fullScreenChanged();
    void positionChanged();
    void rateChanged();
    void minimumRateChanged();
    void maximumRateChanged();
    void volumeChanged();
    void initialFetchFailed();
    void initialFetchFinished();

public Q_SLOTS:
    void raise();
    void quit();

    void previous();
    void next();

    void play();
    void pause();
    void playPause();
    void stop();

    void seek(qlonglong offset);
    void setPosition(const QString &trackId, qlonglong position);

    void openUrl(const QUrl &url);

private:
    QString m_serviceName;

    OrgFreedesktopDBusPropertiesInterface *m_propsInterface;
    OrgMprisMediaPlayer2Interface *m_interface;
    OrgMprisMediaPlayer2PlayerInterface *m_playerInterface;
    int m_fetchesPending;

    QString m_identity;
    QString m_iconName;

    Capabilities m_capabilities;

    QQmlPropertyMap *m_metadata;

    QString m_status;

    bool m_fullScreen;

    QDateTime m_lastPosUpdate;
    qlonglong m_position;
    qreal m_rate;
    qreal m_minimumRate;
    qreal m_maximumRate;
    qreal m_volume;

    void retrieveData();

    void setCapabilities(Capabilities cap);
    void setMetadata(const Metadata &map);

    void calculatePosition();

private Q_SLOTS:
    void propertiesFinished(QDBusPendingCallWatcher *watcher);
    void updateFromMap(const QVariantMap &map);
    void copyProperty(const QString &name, const QVariant &value,
                      const QMetaType::Type &expectedType);
};

#endif // LIRI_MPRIS2PLAYER_H
