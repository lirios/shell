// SPDX-FileCopyrightText: 2017 Michael Spencer <sonrisesoftware@gmail.com>
//
// SPDX-License-Identifier: GPL-3.0-or-later

#pragma once

#include <QtCore/QObject>
#include <QtCore/QSet>
#include <QtCore/QStringList>
#include <QtQml>
#include <QtQml/QQmlListProperty>

#include <LiriXdg/DesktopFile>

namespace Aurora {

namespace Compositor {

class WaylandClient;

} // namespace Compositor

} // namespace Aurora

class ApplicationManager;

class DesktopFileAction : public QObject
{
    Q_OBJECT
    Q_PROPERTY(QString name READ name CONSTANT)
    Q_PROPERTY(QString comment READ comment CONSTANT)
    Q_PROPERTY(QString command READ command CONSTANT)

    friend class Application;
public:
    explicit DesktopFileAction(QObject *parent = nullptr);

    QString name() const;
    QString comment() const;
    QString command() const;

private:
    QString m_name;
    QString m_comment;
    QString m_command;
};

class Application : public QObject
{
    Q_OBJECT

    // Properties from the desktop file

    Q_PROPERTY(QString appId READ appId CONSTANT)
    Q_PROPERTY(bool valid READ isValid CONSTANT)

    Q_PROPERTY(QString name READ name NOTIFY dataChanged)
    Q_PROPERTY(QString genericName READ genericName NOTIFY dataChanged)
    Q_PROPERTY(QString comment READ comment NOTIFY dataChanged)
    Q_PROPERTY(QString iconName READ iconName NOTIFY dataChanged)
    Q_PROPERTY(QStringList categories READ categories NOTIFY dataChanged)

    Q_PROPERTY(State state READ state NOTIFY stateChanged)
    Q_PROPERTY(bool running READ isRunning NOTIFY stateChanged)
    Q_PROPERTY(bool starting READ isStarting NOTIFY stateChanged)
    Q_PROPERTY(bool active READ isActive NOTIFY activeChanged)
    Q_PROPERTY(bool pinned READ isPinned WRITE setPinned NOTIFY pinnedChanged)
    Q_PROPERTY(int count READ count NOTIFY countChanged)
    Q_PROPERTY(int progress READ progress NOTIFY progressChanged)

    Q_PROPERTY(QQmlListProperty<DesktopFileAction> actions READ actions CONSTANT)

    friend class ApplicationManager;

public:
    enum State
    {
        //! The application is not running
        NotRunning,
        //! The application was launched and it's starting up.
        Starting,
        //! The application is currently running.
        Running,
        //! The application is in the background and it was suspended by
        //! the system to save resources.
        Suspended,
        //! The application is in the background and its activity was
        //! stopped by the system in order to save resources, acts as
        //! being closed but the state is saved on disk and can be
        //! restored.
        Stopped
    };
    Q_ENUM(State)

    explicit Application(const QString &appId, const QStringList &categories, QObject *parent = nullptr);
    ~Application();

    bool isValid() const { return m_desktopFile != nullptr && m_desktopFile->isValid(); }

    bool hasCategory(const QString &category) const;

    QString name() const { return m_desktopFile->name(); }
    QString genericName() const { return m_desktopFile->genericName(); }
    QString comment() const { return m_desktopFile->comment(); }
    QString iconName() const { return m_desktopFile->iconName(); }
    QStringList categories() const { return m_categories; }

    /*!
     * \brief Application state.
     *
     * Holds the current state of the application.
     */
    State state() const { return m_state; }

    /*!
     * \brief Application identifier.
     *
     * Holds the application identifier according to the Freedesktop
     * Desktop Entry specification.
     */
    QString appId() const { return m_appId; }

    /*!
     * \brief Desktop entry file name.
     *
     * Returns the desktop entry file name.
     */
    Liri::DesktopFile *desktopFile() const { return m_desktopFile; }

    /*!
     * \brief Application active state.
     *
     * Returns whether the application is currently active or not.
     */
    bool isActive() const { return m_active; }

    bool isRunning() const { return m_state == Application::Running; }
    bool isStarting() const { return m_state == Application::Starting; }

    int count() const { return m_count; }
    int progress() const { return m_progress; }

    bool isPinned() const { return m_pinned; }

    QQmlListProperty<DesktopFileAction> actions();

public Q_SLOTS:
    void setPinned(bool pinned);

    Q_INVOKABLE bool launch(const QStringList &urls);
    Q_INVOKABLE bool quit();

    Q_INVOKABLE bool launch() { return launch(QStringList()); }

protected Q_SLOTS:
    void setState(Application::State state);
    void setActive(bool active);

Q_SIGNALS:
    void stateChanged();
    void activeChanged();
    void pinnedChanged();
    void countChanged();
    void progressChanged();
    void launched();
    void dataChanged();

protected:
    QSet<qint64> m_pids;
    QVector<Aurora::Compositor::WaylandClient *> m_clients;

private:
    QString m_appId;
    QStringList m_categories;
    Liri::DesktopFile *m_desktopFile = nullptr;
    QVector<DesktopFileAction *> m_actions;
    bool m_active = false;
    bool m_pinned = false;
    int m_count = 0;
    int m_progress = 0;
    State m_state = NotRunning;

    void addClient(Aurora::Compositor::WaylandClient *client);
};

QML_DECLARE_TYPE(DesktopFileAction)
