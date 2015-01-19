/****************************************************************************
 * This file is part of Hawaii.
 *
 * Copyright (C) 2015 Pier Luigi Fiorini <pierluigi.fiorini@gmail.com>
 *
 * Author(s):
 *    Pier Luigi Fiorini
 *
 * $BEGIN_LICENSE:LGPL2.1+$
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License as published by
 * the Free Software Foundation, either version 2.1 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 *
 * $END_LICENSE$
 ***************************************************************************/

#include <QtQml/QQmlExtensionPlugin>
#include <QtQml/QQmlComponent>

#include "theme.h"
#include "themesettings.h"
#include "units.h"

class HawaiiThemesPlugin : public QQmlExtensionPlugin
{
    Q_OBJECT
    Q_PLUGIN_METADATA(IID "org.qt-project.Qt.QQmlExtensionInterface/1.0")
public:
    void registerTypes(const char *uri);

    static QObject *themeProvider(QQmlEngine *engine, QJSEngine *jsEngine);
    static QObject *themeSettingsProvider(QQmlEngine *engine, QJSEngine *jsEngine);
    static QObject *unitsProvider(QQmlEngine *engine, QJSEngine *jsEngine);
};

void HawaiiThemesPlugin::registerTypes(const char *uri)
{
    // @uri Hawaii.Themes
    qmlRegisterSingletonType<Theme>(uri, 1, 0, "Theme",
                                    HawaiiThemesPlugin::themeProvider);
    qmlRegisterSingletonType<ThemeSettings>(uri, 1, 0, "ThemeSettings",
                                            HawaiiThemesPlugin::themeSettingsProvider);
    qmlRegisterSingletonType<Units>(uri, 1, 0, "Units",
                                    HawaiiThemesPlugin::unitsProvider);
}

QObject *HawaiiThemesPlugin::themeProvider(QQmlEngine *engine, QJSEngine *jsEngine)
{
    Q_UNUSED(jsEngine);

    return new Theme(engine);
}

QObject *HawaiiThemesPlugin::themeSettingsProvider(QQmlEngine *engine, QJSEngine *jsEngine)
{
    Q_UNUSED(engine);
    Q_UNUSED(jsEngine);

    return new ThemeSettings();
}

QObject *HawaiiThemesPlugin::unitsProvider(QQmlEngine *engine, QJSEngine *jsEngine)
{
    Q_UNUSED(engine);
    Q_UNUSED(jsEngine);

    return new Units();
}

#include "plugin.moc"
