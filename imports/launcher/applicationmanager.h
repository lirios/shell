/****************************************************************************
 * This file is part of Liri.
 *
 * Copyright (C) 2015-2016 Pier Luigi Fiorini <pierluigi.fiorini@gmail.com>
 * Copyright (C) 2016 Michael Spencer <sonrisesoftware@gmail.com>
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

#include <GreenIsland/Server/ApplicationManager>
#include <Vibe/Settings/QGSettings>
#include <QtCore/QLoggingCategory>
#include <QtCore/QObject>

class QDomElement;
class Application;

using namespace Vibe;

Q_DECLARE_LOGGING_CATEGORY(APPLICATION_MANAGER)

class ApplicationManager : public QObject
{
    Q_OBJECT

    Q_PROPERTY(GreenIsland::Server::ApplicationManager *applicationManager READ applicationManager WRITE setApplicationManager NOTIFY applicationManagerChanged)
public:
    ApplicationManager(QObject *parent = nullptr);

    GreenIsland::Server::ApplicationManager *applicationManager() const;

    Application *getApplication(const QString &appId);

    QList<Application *> applications() const;
    QList<Application *> pinnedApps() const;

public Q_SLOTS:
    void refresh();
    void quit(const QString &appId);
    void launch(const QString &appId);

    void setApplicationManager(GreenIsland::Server::ApplicationManager *appMan);

Q_SIGNALS:
    void applicationManagerChanged();
    void applicationLaunched(Application *app);
    void refreshed();
    void applicationAdded(Application *app);

private Q_SLOTS:
    void handleApplicationAdded(QString appId, pid_t pid);
    void handleApplicationRemoved(QString appId, pid_t pid);
    void handleApplicationFocused(QString appId);

private:
    GreenIsland::Server::ApplicationManager *m_appMan = nullptr;
    QGSettings *m_settings = nullptr;
    QMap<QString, Application *> m_apps;

    void readAppLink(const QDomElement &xml, const QString &categoryName);
};
