// SPDX-FileCopyrightText: 2021 Pier Luigi Fiorini <pierluigi.fiorini@gmail.com>
//
// SPDX-License-Identifier: GPL-3.0-or-later

#include <QStandardPaths>

#include <LiriXdg/DesktopFile>

#include "appidandicon.h"

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

AppIdAndIcon::AppIdAndIcon(QObject *parent)
    : QObject(parent)
{
}

QString AppIdAndIcon::appId() const
{
    return m_appId;
}

void AppIdAndIcon::setAppId(const QString &appId)
{
    if (m_appId == appId)
        return;

    m_appId = appId;
    if (correctAppIds.contains(m_appId))
        m_canonicalAppId = correctAppIds[m_appId];
    else
        m_canonicalAppId = appId;
    Q_EMIT appIdChanged();

    updateIconName();
}

QString AppIdAndIcon::canonicalAppId() const
{
    return m_canonicalAppId;
}

QString AppIdAndIcon::iconName() const
{
    return m_iconName;
}

void AppIdAndIcon::updateIconName()
{
    QString fileName = m_appId + QStringLiteral(".desktop");
    QString path =
            QStandardPaths::locate(QStandardPaths::ApplicationsLocation,
                                   fileName);
    if (!path.isEmpty()) {
        Liri::DesktopFile desktopFile(path);
        if (desktopFile.iconName() != m_iconName) {
            m_iconName = desktopFile.iconName();
            Q_EMIT iconNameChanged();
        }
    }
}
