// SPDX-FileCopyrightText: 2019 Pier Luigi Fiorini <pierluigi.fiorini@gmail.com>
//
// SPDX-License-Identifier: GPL-3.0-or-later

#include <QStandardPaths>

#include <LiriXdg/DesktopFile>

#include "waylandwindow.h"

WaylandWindow::WaylandWindow(QObject *parent)
    : QObject(parent)
{
}

QString WaylandWindow::appId() const
{
    return m_appId;
}

void WaylandWindow::setAppId(const QString &appId)
{
    if (m_appId == appId)
        return;

    m_appId = appId;
    emit appIdChanged();

    updateIconName();
}

QString WaylandWindow::iconName() const
{
    return m_iconName;
}

void WaylandWindow::updateIconName()
{
    QString fileName = m_appId + QStringLiteral(".desktop");
    QString path =
            QStandardPaths::locate(QStandardPaths::ApplicationsLocation,
                                   fileName);
    if (!path.isEmpty()) {
        Liri::DesktopFile desktopFile(path);
        if (desktopFile.iconName() != m_iconName) {
            m_iconName = desktopFile.iconName();
            emit iconNameChanged();
        }
    }
}
