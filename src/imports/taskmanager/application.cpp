// SPDX-FileCopyrightText: 2017 Michael Spencer <sonrisesoftware@gmail.com>
// SPDX-FileCopyrightText: 2018-2021 Pier Luigi Fiorini <pierluigi.fiorini@gmail.com>
//
// SPDX-License-Identifier: GPL-3.0-or-later

#include <QDBusConnection>
#include <QDBusInterface>
#include <QGuiApplication>
#include <QTimer>
#include <qpa/qplatformnativeinterface.h>

#include "application.h"
#include "usagetracker.h"

Q_LOGGING_CATEGORY(lcTaskManager, "liri.taskmanager", QtInfoMsg)

// Applications have 5 seconds to start up before the start animation ends
#define MAX_APPLICATION_STARTUP_TIME (5 * 1000)

/*
 * ApplicationAction
 */

ApplicationAction::ApplicationAction(QObject *parent)
    : QObject(parent)
{
}

QString ApplicationAction::name() const
{
    return m_name;
}

QString ApplicationAction::comment() const
{
    return m_comment;
}

QString ApplicationAction::command() const
{
    return m_command;
}

bool ApplicationAction::execute()
{
    const QDBusConnection bus = QDBusConnection::sessionBus();
    QDBusInterface interface(QStringLiteral("io.liri.Launcher"),
                             QStringLiteral("/io/liri/Launcher"),
                             QStringLiteral("io.liri.Launcher"), bus);
    QDBusMessage msg = interface.call(QStringLiteral("LaunchCommand"),
                                      m_command);
    return msg.arguments().at(0).toBool();
}

/*
 * ApplicationActionsModel
 */

ApplicationActionsModel::ApplicationActionsModel(QObject *parent)
    : QAbstractListModel(parent)
{
}

ApplicationActionsModel::~ApplicationActionsModel()
{
    clear();
}

QHash<int, QByteArray> ApplicationActionsModel::roleNames() const
{
    QHash<int, QByteArray> roles;
    roles.insert(NameRole, "name");
    roles.insert(CommentRole, "comment");
    roles.insert(CommandRole, "command");
    roles.insert(ActionRole, "action");
    return roles;
}

int ApplicationActionsModel::rowCount(const QModelIndex &parent) const
{
    Q_UNUSED(parent)
    return m_actions.size();
}

QVariant ApplicationActionsModel::data(const QModelIndex &index, int role) const
{
    if (!index.isValid())
        return QVariant();

    ApplicationAction *action = m_actions.at(index.row());
    if (!action)
        return QVariant();

    switch (role) {
    case Qt::DisplayRole:
    case NameRole:
        return action->name();
    case CommentRole:
        return action->comment();
    case CommandRole:
        return action->command();
    case ActionRole:
        return QVariant::fromValue(action);
    }

    return QVariant();
}

void ApplicationActionsModel::addAction(ApplicationAction *action)
{
    m_actions.append(action);
}

void ApplicationActionsModel::clear()
{
    while (!m_actions.isEmpty())
        m_actions.takeFirst()->deleteLater();
}

/*
 * Application
 */

Application::Application(const QString &appId, QObject *parent)
    : QObject(parent)
    , m_appId(appId)
    , m_actionsModel(new ApplicationActionsModel(this))
{
    refresh();
}

Application::~Application()
{
    delete m_desktopFile;
}

QString Application::appId() const
{
    return m_appId;
}

bool Application::isValid() const
{
    return m_desktopFile && m_desktopFile->isValid();
}

QString Application::name() const
{
    return m_desktopFile ? m_desktopFile->name() : QString();
}

QString Application::genericName() const
{
    return m_desktopFile ? m_desktopFile->genericName() : QString();
}

QString Application::comment() const
{
    return m_desktopFile ? m_desktopFile->comment() : QString();
}

QString Application::iconName() const
{
    return m_desktopFile ? m_desktopFile->iconName() : QString();
}

QStringList Application::categories() const
{
    return m_categories;
}

bool Application::hasCategory(const QString &category) const
{
    return m_categories.contains(category);
}

Application::State Application::state() const
{
    return m_state;
}

bool Application::isRunning() const
{
    return m_state == Running;
}

bool Application::isStarting() const
{
    return m_state == Starting;
}

bool Application::isActive() const
{
    return m_active;
}

bool Application::isPinned() const
{
    return m_pinned;
}

void Application::setPinned(bool pinned)
{
    if (m_pinned == pinned)
        return;

    m_pinned = pinned;
    emit pinnedChanged(pinned);
}

int Application::count() const
{
    return m_count;
}

int Application::progress() const
{
    return m_progress;
}

ApplicationActionsModel *Application::actionsModel() const
{
    return m_actionsModel;
}

void Application::refresh()
{
    const bool oldIsValid = isValid();
    const bool firstTime = m_desktopFile == nullptr;

    m_desktopFile = Liri::DesktopFileCache::getFile(m_appId + QStringLiteral(".desktop"));
    if (oldIsValid != isValid())
        emit validChanged();
    if (m_desktopFile && m_desktopFile->isValid()) {
        m_actionsModel->clear();

        const auto actionNames = m_desktopFile->actionNames();
        for (const auto &actionName : actionNames) {
            Liri::DesktopFileAction desktopFileAction = m_desktopFile->action(actionName);
            ApplicationAction *action = new ApplicationAction();
            action->m_name = desktopFileAction.name();
            action->m_comment = desktopFileAction.comment();
            action->m_command = desktopFileAction.exec();
            m_actionsModel->addAction(action);
        }

        if (!firstTime)
            emit dataChanged();
    }
}

void Application::setRectangle(QWindow *window, const QRect &rect)
{
    for (auto *handle : qAsConst(m_handles))
        handle->setRectangle(window, rect);
}

void Application::setMinimized()
{
    for (auto *handle : qAsConst(m_handles))
        handle->setMinimized();
}

void Application::unsetMinimized()
{
    for (auto *handle : qAsConst(m_handles))
        handle->unsetMinimized();
}

void Application::toggleMinimized()
{
    if (m_active) {
        setMinimized();
        setActive(false);
    } else {
        unsetMinimized();
        activate();
    }
}

void Application::activate()
{
    void *rawSeat = QGuiApplication::platformNativeInterface()->nativeResourceForIntegration("wl_seat");
    auto *seat = static_cast<struct ::wl_seat *>(rawSeat);
    for (auto *handle : qAsConst(m_handles))
        handle->activate(seat);
}

bool Application::launch(const QStringList &urls)
{
    if (!m_desktopFile->isValid())
        return false;

    if (isRunning())
        return true;

    const QDBusConnection bus = QDBusConnection::sessionBus();
    QDBusInterface interface(QStringLiteral("io.liri.Launcher"),
                             QStringLiteral("/io/liri/Launcher"),
                             QStringLiteral("io.liri.Launcher"), bus);
    QDBusMessage msg;
    if (urls.isEmpty())
        msg = interface.call(QStringLiteral("LaunchDesktopFile"),
                             m_desktopFile->fileName());
    else
        msg = interface.call(QStringLiteral("LaunchDesktopFile"),
                             m_desktopFile->fileName(),
                             urls);
    bool ran = msg.arguments().at(0).toBool();

    if (ran) {
        setState(Starting);

        QTimer::singleShot(MAX_APPLICATION_STARTUP_TIME, this, [=]() {
            if (isStarting())
                setState(NotRunning);
        });

        emit launched();
    }

    return ran;
}

bool Application::quit()
{
    if (!isRunning())
        return false;

    for (auto *handle : qAsConst(m_handles))
        handle->close();

    return true;
}

void Application::setState(State state)
{
    if (m_state == state)
        return;

    if (state == NotRunning)
        setActive(false);

    m_state = state;
    emit stateChanged(state);
}

void Application::setActive(bool active)
{
    if (m_active == active)
        return;

    m_active = active;
    emit activeChanged(active);

    // Track when the app is activated
    if (isValid() && !m_desktopFile->noDisplay())
        UsageTracker::instance()->applicationFocused(m_appId);
}

void Application::addHandle(Aurora::Client::WlrForeignToplevelHandleV1 *handle)
{
    connect(handle, &Aurora::Client::WlrForeignToplevelHandleV1::closed,
            this, &Application::handleClosed);

    m_handles.append(handle);

    if (m_state < Running)
        setState(Running);

    // Track when the app is launched
    if (isValid() && !m_desktopFile->noDisplay()) {
        static bool firstTime = true;

        if (firstTime) {
            firstTime = false;
            UsageTracker::instance()->applicationLaunched(m_appId);
        }
    }
}

void Application::removeHandle(Aurora::Client::WlrForeignToplevelHandleV1 *handle)
{
    disconnect(handle, &Aurora::Client::WlrForeignToplevelHandleV1::closed,
               this, &Application::handleClosed);

    m_handles.removeOne(handle);

    if (m_handles.size() == 0)
        setState(NotRunning);
}

void Application::handleClosed()
{
    auto *handle = qobject_cast<Aurora::Client::WlrForeignToplevelHandleV1 *>(sender());
    if (!handle)
        return;

    removeHandle(handle);
}
