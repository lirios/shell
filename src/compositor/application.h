/****************************************************************************
 * This file is part of Liri.
 *
 * Copyright (C) 2018 Pier Luigi Fiorini
 *
 * Author(s):
 *    Pier Luigi Fiorini <pierluigi.fiorini@gmail.com>
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

#pragma once

#include <QEvent>
#include <QObject>
#include <QUrl>

QT_FORWARD_DECLARE_CLASS(QQmlApplicationEngine)

class MultimediaKeys;
class ProcessLauncher;
class ScreenSaver;
class SessionManager;

class Application : public QObject
{
    Q_OBJECT
public:
    explicit Application(QObject *parent = nullptr);

    bool isAutostartEnabled() const;
    void setAutostartEnabled(bool enabled);

    QString screenConfigurationFileName() const;
    void setScreenConfigurationFileName(const QString &fileName);

    void setUrl(const QUrl &url);

    static QString systemInformation();

protected:
    void customEvent(QEvent *event) override;

private:
    QUrl m_url;
    QQmlApplicationEngine *m_appEngine;
    MultimediaKeys *m_multimediaKeys;
    ProcessLauncher *m_launcher;
    SessionManager *m_sessionManager;
    bool m_failSafe;
    bool m_started;
    bool m_autostartEnabled;
    QString m_screenConfigFileName;

    void verifyXdgRuntimeDir();

private Q_SLOTS:
    void startup();
    void shutdown();
    void autostart();
    void unixSignal();
    void objectCreated(QObject *object, const QUrl &);
};

class StartupEvent : public QEvent
{
public:
    StartupEvent();
};
