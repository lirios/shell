// SPDX-FileCopyrightText: 2017 Michael Spencer <sonrisesoftware@gmail.com>
// SPDX-FileCopyrightText: 2018 Pier Luigi Fiorini <pierluigi.fiorini@gmail.com>
//
// SPDX-License-Identifier: GPL-3.0-or-later

#pragma once

#include <QtCore/QLoggingCategory>
#include <QtCore/QAbstractListModel>

#include <Qt5GSettings/QGSettings>

class QDomElement;
class Application;

using namespace QtGSettings;

Q_DECLARE_LOGGING_CATEGORY(APPLICATION_MANAGER)

class ApplicationManager : public QAbstractListModel
{
    Q_OBJECT
public:
    enum Roles
    {
        AppIdRole = Qt::UserRole + 1,
        ApplicationRole,
        NameRole,
        GenericNameRole,
        CommentRole,
        IconNameRole,
        CategoriesRole,
        FilterInfoRole,
        PinnedRole,
        PinnedIndexRole,
        RunningRole,
        StartingRole,
        ActiveRole,
        HasWindowsRole,
        HasCountRole,
        CountRole,
        HasProgressRole,
        ProgressRole,
        ActionsRole
    };
    Q_ENUM(Roles)

    explicit ApplicationManager(QObject *parent = nullptr);
    virtual ~ApplicationManager();

    QHash<int, QByteArray> roleNames() const override;

    int rowCount(const QModelIndex &parent = QModelIndex()) const override;
    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;

    Application *findApplication(const QString &appId);
    Application *newApplication(const QString &appId);

    Q_INVOKABLE void registerShellSurface(QObject *shellSurface);
    Q_INVOKABLE void unregisterShellSurface(QObject *shellSurface);
    Q_INVOKABLE void focusShellSurface(QObject *shellSurface);

    Q_INVOKABLE QString canonicalizeAppId(const QString &appId);

    Q_INVOKABLE Application *get(int index) const;

    Q_INVOKABLE QString getIconName(const QString &appId);
    Q_INVOKABLE int indexFromAppId(const QString &appId) const;

    static void refresh(ApplicationManager *manager);

public Q_SLOTS:
    void launch(const QString &appId);
    void quit(const QString &appId);

Q_SIGNALS:
    void refreshed();
    void applicationAdded(Application *app);
    void applicationRemoved(Application *app);
    void applicationPinned(Application *app);
    void applicationUnpinned(Application *app);
    void applicationLaunched(Application *app);
    void shellSurfaceFocused(QObject *shellSurface);

private:
    QGSettings *m_settings = nullptr;
    QList<Application *> m_apps;
    QMap<QObject *, QString> m_shellSurfaces;

private Q_SLOTS:
    void addApp(const QString &appId, const QString &categoryName);
    void removeApp(QObject *object);
};
