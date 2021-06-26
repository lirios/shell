// SPDX-FileCopyrightText: 2018 Pier Luigi Fiorini
//
// SPDX-License-Identifier: GPL-3.0-or-later

#include <QtQml/QtQml>

#include "application.h"
#include "applicationmanager.h"
#include "categoriesmodel.h"
#include "frequentmodel.h"
#include "launchermodel.h"
#include "pagemodel.h"

class LauncherPlugin : public QQmlExtensionPlugin
{
    Q_OBJECT
    Q_PLUGIN_METADATA(IID "org.qt-project.Qt.QQmlExtensionInterface")

public:
    void registerTypes(const char *uri)
    {
        // @uri Liri.Launcher
        Q_ASSERT(QLatin1String(uri) == QLatin1String("Liri.Launcher"));

        qmlRegisterType<ApplicationManager>(uri, 1, 0, "ApplicationManager");
        qmlRegisterType<CategoriesModel>(uri, 1, 0, "CategoriesModel");
        qmlRegisterType<LauncherModel>(uri, 1, 0, "LauncherModel");
        qmlRegisterType<PageModel>(uri, 1, 0, "PageModel");
        qmlRegisterType<FrequentAppsModel>(uri, 1, 0, "FrequentAppsModel");
        qmlRegisterUncreatableType<Application>(uri, 1, 0, "Application",
                                                QStringLiteral("Cannot instantiate Application"));
        qmlRegisterUncreatableType<DesktopFileAction>(uri, 1, 0, "DesktopFileAction",
                                                      QStringLiteral("Cannot instantiate DesktopFileAction"));
    }
};

#include "plugin.moc"
