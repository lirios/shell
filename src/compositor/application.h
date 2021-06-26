// SPDX-FileCopyrightText: 2018 Pier Luigi Fiorini
//
// SPDX-License-Identifier: GPL-3.0-or-later

#pragma once

#include <QEvent>
#include <QObject>
#include <QUrl>

QT_FORWARD_DECLARE_CLASS(QQmlApplicationEngine)

class ScreenSaver;
class SessionManager;

class Application : public QObject
{
    Q_OBJECT
public:
    explicit Application(QObject *parent = nullptr);

    QString screenConfigurationFileName() const;
    void setScreenConfigurationFileName(const QString &fileName);

    void setUrl(const QUrl &url);

protected:
    void customEvent(QEvent *event) override;

private:
    QUrl m_url;
    QQmlApplicationEngine *m_appEngine;
    SessionManager *m_sessionManager;
    bool m_failSafe;
    bool m_started;
    QString m_screenConfigFileName;

    void verifyXdgRuntimeDir();

private Q_SLOTS:
    void startup();
    void shutdown();
    void objectCreated(QObject *object, const QUrl &);
};

class StartupEvent : public QEvent
{
public:
    StartupEvent();
};
