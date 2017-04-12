/****************************************************************************
 * This file is part of Liri.
 *
 * Copyright (C) 2017 Pier Luigi Fiorini <pierluigi.fiorini@gmail.com>
 * Copyright (C) 2017 Michael Spencer <sonrisesoftware@gmail.com>
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

#include <QtCore/QFileInfo>
#include <QtDBus/QDBusConnection>
#include <QtDBus/QDBusInterface>
#include <QtGui/QIcon>
#include <QtWaylandCompositor/QWaylandSurface>
#include <qt5xdg/xdgmenu.h>
#include <qt5xdg/xmlhelper.h>

#include "application.h"
#include "applicationmanager.h"
#include "usagetracker.h"

Q_LOGGING_CATEGORY(APPLICATION_MANAGER, "liri.launcher.applicationmanager")

bool appLessThan(Application *e1, Application *e2)
{
    return e1->name() < e2->name();
}

ApplicationManager::ApplicationManager(QObject *parent)
    : QObject(parent)
{
    m_settings = new QGSettings(QStringLiteral("io.liri.desktop.panel"),
                                QStringLiteral("/io/liri/desktop/panel/"), this);

    refresh();
}

Application *ApplicationManager::getApplication(const QString &appId)
{
    Application *app = m_apps[appId];

    if (app != nullptr)
        return app;

    app = new Application(appId, QStringList(), this);
    m_apps[app->appId()] = app;

    connect(app, &Application::launched, this, [this, app] {
        Q_EMIT applicationLaunched(app);
    });
    connect(app, &Application::pinnedChanged, this, [this, app] {
        // Currently pinned launchers
        QStringList pinnedLaunchers =
            m_settings->value(QStringLiteral("pinnedLaunchers")).toStringList();
        bool changed = false;

        // Add or remove from the pinned launchers
        if (app->isPinned()) {
            if (!pinnedLaunchers.contains(app->appId())) {
                pinnedLaunchers.append(app->appId());
                changed = true;
            }
        } else {
            changed = pinnedLaunchers.removeOne(app->appId());
        }
        if (changed)
            m_settings->setValue(QStringLiteral("pinnedLaunchers"), pinnedLaunchers);
    });

    Q_EMIT applicationAdded(app);

    return app;
}

void ApplicationManager::launch(const QString &appId)
{
    getApplication(appId)->launch();
}

void ApplicationManager::refresh()
{
    qDeleteAll(m_apps);
    m_apps.clear();

    XdgMenu xdgMenu;
    //xdgMenu.setLogDir("/tmp/");
    xdgMenu.setEnvironments(QStringList() << QStringLiteral("Liri")
                                          << QStringLiteral("X-Liri"));

    const QString menuFileName = XdgMenu::getMenuFileName();

    qCDebug(APPLICATION_MANAGER) << "Menu file name:" << menuFileName;
    if (!xdgMenu.read(menuFileName)) {
        qCWarning(APPLICATION_MANAGER, "Failed to read menu \"%s\": %s",
                  qPrintable(menuFileName),
                  qPrintable(xdgMenu.errorString()));
        return;
    }

    QDomElement xml = xdgMenu.xml().documentElement();

    DomElementIterator it(xml, QString());
    while (it.hasNext()) {
        QDomElement xml = it.next();

        if (xml.tagName() == QStringLiteral("Menu")) {
            QString categoryName = xml.attribute(QStringLiteral("name"));

            DomElementIterator it(xml, QString());
            while (it.hasNext()) {
                QDomElement xml = it.next();

                if (xml.tagName() == QStringLiteral("AppLink"))
                    readAppLink(xml, categoryName);
            }
        }
    }

    QStringList pinnedLaunchers = m_settings->value("pinnedLaunchers").toStringList();
    for (const QString &appId : qAsConst(pinnedLaunchers))
        getApplication(appId)->setPinned(true);

    Q_EMIT refreshed();
}

void ApplicationManager::quit(const QString &appId)
{
    if (m_apps.contains(appId))
        m_apps[appId]->quit();
}

QList<Application *> ApplicationManager::applications() const
{
    QList<Application *> apps;

    for (Application *app : qAsConst(m_apps)) {
        if (app->categories().count() > 0 && app->isValid())
            apps.append(app);
    }

    qSort(apps.begin(), apps.end(), appLessThan);

    return apps;
}

QList<Application *> ApplicationManager::pinnedApps() const
{
    // We have to look up the pinnedLaunchers list instead of just getting all pinned
    // apps because the apps map isn't sorted
    QList<Application *> apps;
    QStringList pinnedLaunchers = m_settings->value("pinnedLaunchers").toStringList();

    for (Application *app : qAsConst(m_apps)) {
        if (pinnedLaunchers.contains(app->appId()))
            apps.append(app);
    }

    return apps;
}

void ApplicationManager::registerShellSurface(QObject *shellSurface)
{
    QWaylandSurface *surface = shellSurface->property("surface").value<QWaylandSurface *>();
    if (!surface) {
        qCWarning(APPLICATION_MANAGER, "Unable to access surface property");
        return;
    }

    QString appId = shellSurface->property("canonicalAppId").toString();

    Application *app = getApplication(appId);
    app->setState(Application::Running);
    app->m_pids.insert(surface->client()->processId());
    app->addClient(surface->client());

    if (!app->desktopFile()->noDisplay())
        UsageTracker::instance()->applicationLaunched(appId);

    m_shellSurfaces.insertMulti(shellSurface, appId);
}

void ApplicationManager::unregisterShellSurface(QObject *shellSurface)
{
    QString appId = shellSurface->property("canonicalAppId").toString();

    m_shellSurfaces.remove(shellSurface);

    auto values = m_shellSurfaces.values();
    if (!values.contains(appId)) {
        UsageTracker::instance()->applicationFocused(QString());

        Application *app = getApplication(appId);
        app->m_pids.clear();
        app->m_clients.clear();
        app->setState(Application::NotRunning);
    }
}

void ApplicationManager::focusShellSurface(QObject *shellSurface)
{
    QWaylandSurface *surface = shellSurface->property("surface").value<QWaylandSurface *>();
    if (!surface) {
        qCWarning(APPLICATION_MANAGER, "Unable to access surface property");
        return;
    }

    QString appId = shellSurface->property("canonicalAppId").toString();

    Application *app = getApplication(appId);

    app->setActive(true);

    for (Application *app : qAsConst(m_apps)) {
        if (app->appId() != appId)
            app->setActive(false);
    }

    if (!app->desktopFile()->noDisplay())
        UsageTracker::instance()->applicationFocused(appId);
    else
        UsageTracker::instance()->applicationFocused(QString());
}

QString ApplicationManager::canonicalizeAppId(const QString &appId)
{
    return DesktopFile::canonicalAppId(appId);
}

void ApplicationManager::readAppLink(const QDomElement &xml, const QString &categoryName)
{
    QString desktopFile = xml.attribute(QStringLiteral("desktopFile"));
    QString appId = QFileInfo(desktopFile).completeBaseName();

    Application *app = getApplication(appId);

    if (!app->hasCategory(categoryName))
        app->m_categories.append(categoryName);
}
