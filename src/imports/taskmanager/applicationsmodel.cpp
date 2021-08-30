// SPDX-FileCopyrightText: 2019-2021 Pier Luigi Fiorini <pierluigi.fiorini@gmail.com>
//
// SPDX-License-Identifier: GPL-3.0-or-later

#include <QDir>
#include <QFile>
#include <QFileSystemWatcher>
#include <QFuture>
#include <QtConcurrentRun>
#include <QStandardPaths>

#include <LiriXdg/DesktopMenu>

#include "application.h"
#include "applicationsmodel.h"
#include "xdgutils.h"

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

QString canonicalizeAppId(const QString &appId)
{
    if (correctAppIds.contains(appId))
        return correctAppIds[appId];
    return appId;
}

ApplicationsModel::ApplicationsModel(QObject *parent)
    : QAbstractListModel(parent)
    , m_manager(new Aurora::Client::WlrForeignToplevelManagerV1)
{
    // Pinning settings
    m_settings = new QtGSettings::QGSettings(
                QStringLiteral("io.liri.desktop.panel"),
                QStringLiteral("/io/liri/desktop/panel/"), this);

    // Populate the model
    QtConcurrent::run(ApplicationsModel::refresh, this);

    // Repopulate the model every time a new app is installed or uninstalled
    QFileSystemWatcher *watcher = new QFileSystemWatcher(this);
    watcher->addPaths(xdgApplicationsPaths());
    connect(watcher, &QFileSystemWatcher::directoryChanged, this, [this](const QString &) {
        QtConcurrent::run(ApplicationsModel::refresh, this);
    });

    // React to new toplevel windows being created
    connect(m_manager, &Aurora::Client::WlrForeignToplevelManagerV1::toplevel,
            this, [this](Aurora::Client::WlrForeignToplevelHandleV1 *handle) {
        if (!handle->appId().isEmpty()) {
            auto *app = addApplication(handle->appId());
            app->addHandle(handle);
        }

        connect(handle, &Aurora::Client::WlrForeignToplevelHandleV1::appIdChanged,
                this, &ApplicationsModel::handleAppIdChanged);
        connect(handle, &Aurora::Client::WlrForeignToplevelHandleV1::maximizedChanged, this, [this, handle](bool maximized) {
            bool oldHasMaximizedWindow = hasMaximizedWindow();
            if (maximized)
                m_maximizedHandles.append(handle);
            else
                m_maximizedHandles.removeAll(handle);
            if (oldHasMaximizedWindow != hasMaximizedWindow())
                emit hasMaximizedWindowChanged();
        });
        connect(handle, &Aurora::Client::WlrForeignToplevelHandleV1::fullscreenChanged, this, [this, handle](bool fullscreen) {
            bool oldHasFullscreenWindow = hasFullscreenWindow();
            if (fullscreen)
                m_fullscreenHandles.append(handle);
            else
                m_fullscreenHandles.removeAll(handle);
            if (oldHasFullscreenWindow != hasFullscreenWindow())
                emit hasFullscreenWindowChanged();
        });
        connect(handle, &Aurora::Client::WlrForeignToplevelHandleV1::activeChanged, this, [this, handle](bool activated) {
            auto *app = findApplication(handle->appId());
            if (app)
                app->setActive(activated);
        });
    });
}

ApplicationsModel::~ApplicationsModel()
{
    m_manager->deleteLater();

    while (!m_apps.isEmpty())
        delete m_apps.takeFirst();
}

bool ApplicationsModel::hasMaximizedWindow() const
{
    return m_maximizedHandles.size() > 0;
}

bool ApplicationsModel::hasFullscreenWindow() const
{
    return m_fullscreenHandles.size() > 0;
}

QHash<int, QByteArray> ApplicationsModel::roleNames() const
{
    QHash<int, QByteArray> roles;
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

int ApplicationsModel::rowCount(const QModelIndex &parent) const
{
    Q_UNUSED(parent)
    return m_apps.size();
}

QVariant ApplicationsModel::data(const QModelIndex &index, int role) const
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
    case Qt::DisplayRole:
    case NameRole:
        return app->name();
    case AppIdRole:
        return app->appId();
    case ApplicationRole:
        return QVariant::fromValue(app);
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

Application *ApplicationsModel::get(int index) const
{
    if (index < 0 || index >= m_apps.size())
        return nullptr;
    return m_apps.at(index);
}

int ApplicationsModel::indexFromAppId(const QString &appId) const
{
    for (int i = 0; i < m_apps.size(); i++) {
        if (m_apps.at(i)->appId() == appId)
            return i;
    }

    return -1;
}

void ApplicationsModel::activate(const QString &appId)
{
    if (appId.isEmpty()) {
        qCWarning(lcTaskManager, "Activating an application with empty app id has no effect");
        return;
    }

    auto *app = findApplication(appId);
    if (app)
        app->activate();
    else
        qCWarning(lcTaskManager) << "Failed to activate application" << appId;
}

bool ApplicationsModel::launch(const QString &appId, const QStringList &urls)
{
    if (appId.isEmpty()) {
        qCWarning(lcTaskManager, "Launching an application with empty app id has no effect");
        return false;
    }

    auto *app = findApplication(appId);
    if (app)
        return app->launch(urls);
    else
        qCWarning(lcTaskManager) << "Failed to launch application" << appId;

    return false;
}

void ApplicationsModel::refresh(ApplicationsModel *model)
{
    // Identifier of apps that are not in the menu, thus need to be
    // deleted later
    QStringList toRemove;
    for (Application *app : qAsConst(model->m_apps))
        toRemove.append(app->appId());

    // Avoid adding duplicate entries
    QStringList addedEntries;

    Liri::DesktopMenu xdgMenu;
    //xdgMenu.setLogDir(QDir::tempPath());
    xdgMenu.setEnvironments(QStringList() << QStringLiteral("Liri")
                                          << QStringLiteral("X-Liri"));

    const QString menuFileName = Liri::DesktopMenu::getMenuFileName();

    qCDebug(lcTaskManager) << "Menu file name:" << menuFileName;
    if (!xdgMenu.read(menuFileName)) {
        qCWarning(lcTaskManager, "Failed to read menu \"%s\": %s",
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

                    // Add this if we don't have it otherwise refresh it in case the desktop entry has changed
                    auto *app = model->findApplication(appId);
                    if (app)
                        QMetaObject::invokeMethod(app, "refresh");
                    else
                        QMetaObject::invokeMethod(model, "addApplication", Q_ARG(QString, appId), Q_ARG(QString, categoryName));
                }
            }
        }
    }

    // Delete apps that are no longer in the menu
    for (const QString &appId: qAsConst(toRemove)) {
        auto *app = model->findApplication(appId);
        if (app)
            QMetaObject::invokeMethod(model, "removeApplication", Q_ARG(QObject*, qobject_cast<QObject *>(app)));
    }

    // Signal the model was refreshed
    QMetaObject::invokeMethod(model, "refreshed");
}

Application *ApplicationsModel::findApplication(const QString &appId)
{
    for (auto *app : qAsConst(m_apps)) {
        if (app->appId() == appId)
            return app;
    }

    return nullptr;
}

Application *ApplicationsModel::newApplication(const QString &appId)
{
    QString correctAppId = canonicalizeAppId(appId);

    auto *app = new Application(correctAppId);
    app->moveToThread(thread());

    m_apps.append(app);

    // Set initial pinned state
    QStringList pinnedLaunchers = m_settings->value(QStringLiteral("pinnedLaunchers")).toStringList();
    if (pinnedLaunchers.contains(correctAppId))
        app->setPinned(true);

    // React to state changes
    connect(app, &Application::stateChanged, this, [this, app](Application::State) {
        changeData(app, {ActiveRole, StartingRole, RunningRole});
    });
    connect(app, &Application::activeChanged, this, [this, app](bool) {
        changeData(app, {ActiveRole});
    });
    connect(app, &Application::pinnedChanged, this, [this, app](bool pinned) {
        // Currently pinned launchers
        QStringList pinnedLaunchers =
            m_settings->value(QStringLiteral("pinnedLaunchers")).toStringList();
        bool changed = false;

        // Add or remove from the pinned launchers
        if (pinned) {
            if (!pinnedLaunchers.contains(app->appId())) {
                pinnedLaunchers.append(app->appId());
                changed = true;
            }
        } else {
            changed = pinnedLaunchers.removeOne(app->appId());
        }
        if (changed) {
            changeData(app, {PinnedRole, PinnedIndexRole});
            m_settings->setValue(QStringLiteral("pinnedLaunchers"), pinnedLaunchers);
        }
    });
    connect(app, &Application::launched, this, [this, app] {
        Q_EMIT applicationLaunched(app->appId());
    });

    return app;
}

void ApplicationsModel::changeData(Application *app, const QVector<int> &roles)
{
    int i = m_apps.indexOf(app);

    if (i >= 0) {
        QModelIndex modelIndex = index(i);
        emit dataChanged(modelIndex, modelIndex, roles);
    }
}

Application *ApplicationsModel::addApplication(const QString &appId, const QString &category)
{
    auto *app = findApplication(appId);
    if (!app) {
        beginInsertRows(QModelIndex(), m_apps.size(), m_apps.size());
        app = newApplication(appId);
        if (!category.isEmpty() && !app->hasCategory(category))
            app->m_categories.append(category);
        endInsertRows();
    }

    return app;
}

void ApplicationsModel::removeApplication(QObject *object)
{
    auto *app = qobject_cast<Application *>(object);
    if (!app)
        return;

    int index = m_apps.indexOf(app);
    if (index < 0)
        return;

    beginRemoveRows(QModelIndex(), index, index);
    m_apps.removeAt(index);
    endRemoveRows();

    app->deleteLater();
}

void ApplicationsModel::handleAppIdChanged(const QString &oldAppId, const QString &newAppId)
{
    if (newAppId.isEmpty())
        return;

    auto *handle = qobject_cast<Aurora::Client::WlrForeignToplevelHandleV1 *>(sender());
    if (!handle)
        return;

    QString correctOldAppId = canonicalizeAppId(oldAppId);
    QString correctNewAppId = canonicalizeAppId(newAppId);

    // Create a new app if needed
    auto *newApp = addApplication(correctNewAppId);
    newApp->addHandle(handle);

    // When a window is moved to another app we have to remove it from the old one
    if (!oldAppId.isEmpty() && !newAppId.isEmpty()) {
        auto *oldApp = findApplication(correctOldAppId);
        if (oldApp)
            oldApp->removeHandle(handle);
    }
}
