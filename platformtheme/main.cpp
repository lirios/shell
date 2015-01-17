/****************************************************************************
 * This file is part of Hawaii.
 *
 * Copyright (C) 2010-2015 Pier Luigi Fiorini
 *
 * Author(s):
 *    Pier Luigi Fiorini <pierluigi.fiorini@gmail.com>
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

#include <qpa/qplatformthemeplugin.h>

#include "hawaiitheme.h"

class HawaiiThemePlugin : public QPlatformThemePlugin
{
    Q_OBJECT
    Q_PLUGIN_METADATA(IID "org.qt-project.Qt.QPA.QPlatformThemeFactoryInterface.5.1" FILE "hawaiitheme.json")
public:
    explicit HawaiiThemePlugin(QObject *parent = 0);

    QStringList keys() const;
    QPlatformTheme *create(const QString &key, const QStringList &paramList);
};

HawaiiThemePlugin::HawaiiThemePlugin(QObject *parent) :
    QPlatformThemePlugin(parent)
{
}

QStringList HawaiiThemePlugin::keys() const
{
    return QStringList() << "Hawaii";
}

QPlatformTheme *HawaiiThemePlugin::create(const QString &key, const QStringList &paramList)
{
    if (key.toLower() == "hawaii")
        return new HawaiiTheme();
    return 0;
}

#include "main.moc"
