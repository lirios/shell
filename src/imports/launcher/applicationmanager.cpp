// SPDX-FileCopyrightText: 2017 Michael Spencer <sonrisesoftware@gmail.com>
// SPDX-FileCopyrightText: 2018 Pier Luigi Fiorini <pierluigi.fiorini@gmail.com>
//
// SPDX-License-Identifier: GPL-3.0-or-later

#include <QtCore/QFileInfo>
#include <QtCore/QFileSystemWatcher>
#include <QtCore/QFuture>
#include <QtCore/QThread>
#include <QtConcurrent/QtConcurrentRun>
#include <QtDBus/QDBusConnection>
#include <QtDBus/QDBusInterface>
#include <QtGui/QIcon>

#include <LiriAuroraCompositor/WaylandSurface>
#include <LiriXdg/DesktopFile>
#include <LiriXdg/DesktopMenu>

#include "application.h"
#include "applicationmanager.h"
#include "usagetracker.h"
#include "utils.h"

Q_LOGGING_CATEGORY(APPLICATION_MANAGER, "liri.launcher.applicationmanager")

using namespace Aurora::Compositor;

const QMap<QString, QString> correctAppIds = {
    {QStringLiteral("baobob"), QStringLiteral("org.gnome.baobob")},
    {QStringLiteral("cheese"), QStringLiteral("org.gnome.Cheese")},
    {QStringLiteral("corebird"), QStringLiteral("org.baedert.corebird")},
    {QStringLiteral("dconf-editor"), QStringLiteral("ca.desrt.dconf-editor")},
    {QStringLiteral("file-roller"), QStringLiteral("org.gnome.FileRoller")},
    {QStringLiteral("gnome-calendar"), QStringLiteral("org.gnome.Calendar")},
    {QStringLiteral("gnome-disks"), QStringLiteral("org.gnome.DiskUtility")},
    {QStringLiteral("gnome-font-viewer"), QStringLiteral("org.gnome.font-viewer")},
    {QStringLiteral("nautilus"), QStringLiteral("org.gnome.Nautilus")},
    {QStringLiteral("org.kate-editor.kate"), QStringLiteral("org.kde.kate")},
    {QStringLiteral("gedit"), QStringLiteral("org.gnome.gedit")},
    {QStringLiteral("gnome-dictionary"), QStringLiteral("org.gnome.Dictionary")}
};

ApplicationManager::ApplicationManager(QObject *parent)
    : QAbstractListModel(parent)
{
    m_settings = new QGSettings(QStringLiteral("io.liri.desktop.panel"),
                                QStringLiteral("/io/liri/desktop/panel/"), this);

    QtConcurrent::run(ApplicationManager::refresh, this);

    QFileSystemWatcher *watcher = new QFileSystemWatcher(this);
    watcher->addPaths(xdgApplicationsPaths());
    connect(watcher, &QFileSystemWatcher::directoryChanged, this, [this](const QString &) {
        QtConcurrent::run(ApplicationManager::refresh, this);
    });
}

ApplicationManager::~ApplicationManager()
{
    while (!m_apps.isEmpty())
        delete m_apps.takeFirst();
}

QHash<int, QByteArray> ApplicationManager::roleNames() const
{
    QHash<int, QByteArray> roles;
    roles.insert(Qt::DisplayRole, "display");
    roles.insert(Qt::DecorationRole, "decoration");
    roles.insert(AppIdRole, "appId");
    roles.insert(ApplicationRole, "application");
    roles.insert(NameRole, "name");
    roles.insert(GenericNameRole, "genericName");
    roles.insert(CommentRole, "comment");
    roles.insert(IconNameRole, "iconName");
    roles.insert(CategoriesRole, "categories");
    roles.insert(FilterInfoRole, "filterInfo");
    roles.insert(PinnedRole, "pinned");
    roles.insert(PinnedIndexRole, "pinnedIndex");
    roles.insert(RunningRole, "running");
    roles.insert(StartingRole, "starting");
    roles.insert(ActiveRole, "active");
    roles.insert(HasWindowsRole, "hasWindows");
    roles.insert(HasCountRole, "hasCount");
    roles.insert(CountRole, "count");
    roles.insert(HasProgressRole, "hasProgress");
    roles.insert(ProgressRole, "progress");
    return roles;
}

int ApplicationManager::rowCount(const QModelIndex &parent) const
{
    Q_UNUSED(parent)
    return m_apps.size();
}

QVariant ApplicationManager::data(const QModelIndex &index, int role) const
{
    if (!index.isValid())
        return QVariant();

    Application *app = m_apps.at(index.row());
    if (!app)
        return QVariant();

    QStringList pinnedApps =
            m_settings->value(QStringLiteral("pinnedLaunchers")).toStringList();
    int pinnedIndex = pinnedApps.indexOf(app->appId());

    switch (role) {
    case Qt::DecorationRole:
        return QIcon::fromTheme(app->iconName());
    case Qt::DisplayRole:
    case NameRole:
        return app->name();
    case ApplicationRole:
        return qVariantFromValue(app);
    case AppIdRole:
        return app->appId();
    case GenericNameRole:
        return app->genericName();
    case CommentRole:
        return app->comment();
    case IconNameRole:
        return app->iconName();
    case CategoriesRole:
        return app->categories();
    case FilterInfoRole:
        return QString(app->name() + QStringLiteral(" ") + app->genericName() + QStringLiteral(" ") + app->comment());
    case PinnedRole:
        return app->isPinned();
    case PinnedIndexRole:
        return pinnedIndex;
    case RunningRole:
        return app->isRunning();
    case StartingRole:
        return app->isStarting();
    case ActiveRole:
        return app->isActive();
    case HasWindowsRole:
        return false;
    case HasCountRole:
        return app->count() > 0;
    case CountRole:
        return app->count();
    case HasProgressRole:
        return app->progress() >= 0;
    case ProgressRole:
        return app->progress();
    default:
        break;
    }

    return QVariant();
}

Application *ApplicationManager::findApplication(const QString &appId)
{
    for (Application *app : qAsConst(m_apps)) {
        if (app->appId() == appId)
            return app;
    }
    return nullptr;
}

Application *ApplicationManager::newApplication(const QString &appId)
{
    Application *app = new Application(appId, QStringList(), this);
    app->moveToThread(thread());

    m_apps.append(app);

    connect(app, &Application::activeChanged, this, [this, app] {
        int i = m_apps.indexOf(app);

        if (i >= 0) {
            QModelIndex modelIndex = index(i);
            QVector<int> roles;
            roles.append(ApplicationManager::ActiveRole);
            Q_EMIT dataChanged(modelIndex, modelIndex, roles);
        }
    });
    connect(app, &Application::stateChanged, this, [this, app] {
        int i = m_apps.indexOf(app);

        if (i >= 0) {
            QModelIndex modelIndex = index(i);
            QVector<int> roles;
            roles.append(ApplicationManager::ActiveRole);
            roles.append(ApplicationManager::StartingRole);
            roles.append(ApplicationManager::RunningRole);
            Q_EMIT dataChanged(modelIndex, modelIndex, roles);
        }
    });
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
        if (changed) {
            int i = m_apps.indexOf(app);

            if (i >= 0) {
                QModelIndex modelIndex = index(i);
                QVector<int> roles;
                roles.append(ApplicationManager::PinnedRole);
                roles.append(ApplicationManager::PinnedIndexRole);
                Q_EMIT dataChanged(modelIndex, modelIndex, roles);
            }

            m_settings->setValue(QStringLiteral("pinnedLaunchers"), pinnedLaunchers);
        }
    });

    return app;
}

void ApplicationManager::launch(const QString &appId)
{
    Application *app = findApplication(appId);
    if (app)
        app->launch();
}

void ApplicationManager::quit(const QString &appId)
{
    Application *app = findApplication(appId);
    if (app)
        app->quit();
}

void ApplicationManager::refresh(ApplicationManager *manager)
{
    // Identifier of apps that are not in the menu, thus need to be
    // deleted later
    QStringList toRemove;
    for (Application *app : qAsConst(manager->m_apps))
        toRemove.append(app->appId());

    // Avoid adding duplicate entries
    QStringList addedEntries;

    Liri::DesktopMenu xdgMenu;
    //xdgMenu.setLogDir(QDir::tempPath());
    xdgMenu.setEnvironments(QStringList() << QStringLiteral("Liri")
                                          << QStringLiteral("X-Liri"));

    const QString menuFileName = Liri::DesktopMenu::getMenuFileName();

    qCDebug(APPLICATION_MANAGER) << "Menu file name:" << menuFileName;
    if (!xdgMenu.read(menuFileName)) {
        qCWarning(APPLICATION_MANAGER, "Failed to read menu \"%s\": %s",
                  qPrintable(menuFileName),
                  qPrintable(xdgMenu.errorString()));
        return;
    }

    QDomElement doc = xdgMenu.xml().documentElement();
    QDomElement xml = doc.firstChildElement();

    for (; !xml.isNull(); xml = xml.nextSiblingElement()) {
        if (xml.tagName() == QStringLiteral("Menu")) {
            QString categoryName = xml.attribute(QStringLiteral("name"));

            QDomElement child = xml.firstChildElement();
            for (; !child.isNull(); child = child.nextSiblingElement()) {
                if (child.tagName() == QStringLiteral("AppLink")) {
                    QString desktopFileName = child.attribute(QStringLiteral("desktopFile"));
                    QString appId = QFileInfo(desktopFileName).completeBaseName();

                    // App is still in the menu, which means it should not be deleted
                    toRemove.removeOne(appId);

                    // Ignore desktop files that do not exists
                    desktopFileName =
                            QStandardPaths::locate(QStandardPaths::ApplicationsLocation,
                                                   appId + QStringLiteral(".desktop"));
                    if (!QFileInfo::exists(desktopFileName))
                        continue;

                    // Keep track of added entries to avoid duplicates
                    if (addedEntries.contains(appId))
                        continue;
                    addedEntries.append(appId);

                    // Add only valid apps
                    Liri::DesktopFile desktopFile(desktopFileName);
                    if (!desktopFile.isValid())
                        continue;

                    // Add this if we don't have it
                    if (!manager->findApplication(appId))
                        QMetaObject::invokeMethod(manager, "addApp", Q_ARG(QString, appId), Q_ARG(QString, categoryName));
                }
            }
        }
    }

    // Delete apps that are no longer in the menu
    for (const QString &appId: qAsConst(toRemove)) {
        Application *app = manager->findApplication(appId);
        if (app)
            QMetaObject::invokeMethod(manager, "removeApp", Q_ARG(QObject*, qobject_cast<QObject*>(app)));
    }

    // Signal the model was refreshed
    QMetaObject::invokeMethod(manager, "refreshed");
}

void ApplicationManager::registerShellSurface(QObject *shellSurface)
{
    WaylandSurface *surface = shellSurface->property("surface").value<WaylandSurface *>();
    if (!surface) {
        qCWarning(APPLICATION_MANAGER, "Unable to access surface property");
        return;
    }

    QString appId = shellSurface->property("appId").toString();

    Application *app = findApplication(appId);
    if (!app)
        app = newApplication(appId);
    app->setState(Application::Running);
    app->m_pids.insert(surface->client()->processId());
    app->addClient(surface->client());

    if (!app->desktopFile()->noDisplay())
        UsageTracker::instance()->applicationLaunched(appId);

    m_shellSurfaces.insertMulti(shellSurface, appId);
}

void ApplicationManager::unregisterShellSurface(QObject *shellSurface)
{
    m_shellSurfaces.remove(shellSurface);

    QString appId = shellSurface->property("appId").toString();
    if (appId.isEmpty())
        return;

    if (!m_shellSurfaces.values().contains(appId)) {
        UsageTracker::instance()->applicationFocused(QString());

        Application *app = findApplication(appId);
        if (app) {
            app->setState(Application::NotRunning);
            app->m_pids.clear();
            app->m_clients.clear();
        }
    }
}

void ApplicationManager::focusShellSurface(QObject *shellSurface)
{
    WaylandSurface *surface = shellSurface->property("surface").value<WaylandSurface *>();
    if (!surface) {
        qCWarning(APPLICATION_MANAGER, "Unable to access surface property");
        return;
    }

    QString appId = shellSurface->property("appId").toString();

    Application *app = findApplication(appId);
    if (app) {
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

    Q_EMIT shellSurfaceFocused(shellSurface);
}

QString ApplicationManager::canonicalizeAppId(const QString &appId)
{
    if (correctAppIds.contains(appId))
        return correctAppIds[appId];
    return appId;
}

Application *ApplicationManager::get(int index) const
{
    if (index < 0 || index >= m_apps.size())
        return nullptr;
    return m_apps.at(index);
}

QString ApplicationManager::getIconName(const QString &appId)
{
    Application *app = get(indexFromAppId(appId));
    if (app)
        return app->iconName();
    return QString();
}

int ApplicationManager::indexFromAppId(const QString &appId) const
{
    for (int i = 0; i < m_apps.size(); i++) {
        if (m_apps.at(i)->appId() == appId)
            return i;
    }

    return -1;
}

void ApplicationManager::addApp(const QString &appId, const QString &categoryName)
{
    beginInsertRows(QModelIndex(), m_apps.count(), m_apps.count());

    Application *app = newApplication(appId);
    if (!app->hasCategory(categoryName))
        app->m_categories.append(categoryName);

    Q_EMIT applicationAdded(app);

    connect(app, &Application::pinnedChanged, this, [this, app] {
        if (app->isPinned())
            Q_EMIT applicationPinned(app);
        else
            Q_EMIT applicationUnpinned(app);
    });

    QStringList pinnedLaunchers = m_settings->value(QStringLiteral("pinnedLaunchers")).toStringList();
    if (pinnedLaunchers.contains(appId))
        app->setPinned(true);

    endInsertRows();
}

void ApplicationManager::removeApp(QObject *object)
{
    Application *app = qobject_cast<Application *>(object);
    if (!app)
        return;

    int index = m_apps.indexOf(app);
    if (index < 0)
        return;

    beginRemoveRows(QModelIndex(), index, index);
    m_apps.removeAt(index);
    endRemoveRows();

    Q_EMIT applicationRemoved(app);
    app->deleteLater();
}
