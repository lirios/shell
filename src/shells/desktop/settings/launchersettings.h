/****************************************************************************
 * This file is part of Hawaii Shell.
 *
 * Copyright (C) 2013 Pier Luigi Fiorini <pierluigi.fiorini@gmail.com>
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

#ifndef LAUNCHERSETTINGS_H
#define LAUNCHERSETTINGS_H

#include <QtCore/QObject>
#include <QtCore/QStringList>

class QConfiguration;

class LauncherSettings : public QObject
{
    Q_OBJECT
    Q_PROPERTY(LauncherSettings::IconSize iconSize READ iconSize WRITE setIconSize NOTIFY iconSizeChanged)
    Q_PROPERTY(LauncherSettings::Alignment alignment READ alignment WRITE setAlignment NOTIFY alignmentChanged)
    Q_PROPERTY(QStringList favoriteApps READ favoriteApps WRITE setFavoriteApps NOTIFY favoriteAppsChanged)
    Q_ENUMS(IconSize Alignment)
public:
    enum IconSize {
        SmallIconSize = 0,
        MediumIconSize,
        LargeIconSize,
        HugeIconSize
    };

    enum Alignment {
        LeftAlignment = 0,
        RightAlignment,
        BottomAlignment
    };

    LauncherSettings(QObject *parent = 0);
    ~LauncherSettings();

    IconSize iconSize() const;
    void setIconSize(IconSize value);

    Alignment alignment() const;
    void setAlignment(Alignment value);

    QStringList favoriteApps() const;
    void setFavoriteApps(const QStringList &value);

Q_SIGNALS:
    void iconSizeChanged(const LauncherSettings::IconSize &value);
    void alignmentChanged(const LauncherSettings::Alignment &value);
    void favoriteAppsChanged(const QStringList &list);

private:
    QConfiguration *m_configuration;
    IconSize m_iconSize;
    Alignment m_alignment;
    QStringList m_favApps;
};

#endif // LAUNCHERSETTINGS_H
