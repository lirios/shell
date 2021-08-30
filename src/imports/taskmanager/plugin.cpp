// SPDX-FileCopyrightText: 2018-2021 Pier Luigi Fiorini <pierluigi.fiorini@gmail.com>
//
// SPDX-License-Identifier: GPL-3.0-or-later

#include <QtQml/QtQml>

#include "application.h"
#include "applicationsmodel.h"
#include "categoriesmodel.h"
#include "frequentmodel.h"
#include "pagemodel.h"
#include "tasksmodel.h"

class TaskManagerPlugin : public QQmlExtensionPlugin
{
    Q_OBJECT
    Q_PLUGIN_METADATA(IID "org.qt-project.Qt.QQmlExtensionInterface")

public:
    void registerTypes(const char *uri) override
    {
        // @uri Liri.Shell.TaskManager
        Q_ASSERT(strcmp(uri, "Liri.Shell.TaskManager") == 0);

        const int versionMajor = 1;
        const int versionMinor = 0;

        qmlRegisterUncreatableType<Application>(uri, versionMajor, versionMinor, "Application",
                                                QStringLiteral("Cannot instantiate Application"));
        qmlRegisterType<ApplicationsModel>(uri, versionMajor, versionMinor, "ApplicationsModel");
        qmlRegisterType<CategoriesModel>(uri, versionMajor, versionMinor, "CategoriesModel");
        qmlRegisterType<FrequentAppsModel>(uri, versionMajor, versionMinor, "FrequentAppsModel");
        qmlRegisterType<PageModel>(uri, versionMajor, versionMinor, "PageModel");
        qmlRegisterType<TasksModel>(uri, versionMajor, versionMinor, "TasksModel");
    }
};

#include "plugin.moc"
