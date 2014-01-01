/****************************************************************************
 * This file is part of Hawaii Shell.
 *
 * Copyright (C) 2013-2014 Pier Luigi Fiorini <pierluigi.fiorini@gmail.com>
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

#include <QtCore/QDebug>
#include <QtGui/QGuiApplication>
#include <QtGui/QScreen>
#include <QtQml/QQmlEngine>

#include <qpa/qplatformnativeinterface.h>

#include "hawaiishell.h"
#include "shellui.h"
#include "shellscreen.h"

ShellUi::ShellUi(QQmlEngine *engine, QObject *parent)
    : QObject(parent)
    , m_engine(engine)
    , m_lockScreenWindow(0)
    , m_numWorkspaces(0)
{
    // Create grab window
    m_grabWindow = new GrabWindow();
    m_grabWindow->show();
}

ShellUi::~ShellUi()
{
    delete m_grabWindow;
    delete m_lockScreenWindow;
}

QQmlEngine *ShellUi::engine() const
{
    return m_engine;
}

GrabWindow *ShellUi::grabWindow() const
{
    return m_grabWindow;
}

LockScreenWindow *ShellUi::lockScreenWindow() const
{
    return m_lockScreenWindow;
}

void ShellUi::loadScreen(QScreen *screen)
{
    m_screens.append(new ShellScreen(screen, this));
}

void ShellUi::createLockScreenWindow()
{
    if (!m_lockScreenWindow)
        m_lockScreenWindow = new LockScreenWindow();
    m_lockScreenWindow->create();
    m_lockScreenWindow->setWindowType();
    m_lockScreenWindow->show();

    // Synchronization
    while (QCoreApplication::hasPendingEvents())
        QCoreApplication::processEvents();
}

void ShellUi::closeLockScreenWindow()
{
    if (!m_lockScreenWindow)
        return;

    m_lockScreenWindow->deleteLater();
    m_lockScreenWindow = 0;
}

void ShellUi::setNumWorkspaces(int num)
{
    // At least one!
    if (num <= 0) {
        qWarning() << "Setting" << num << "workspaces doesn't make any sense, please set at least one";
        return;
    }

    HawaiiShell *shell = HawaiiShell::instance();

    // Add as many workspaces as needed
    for (; m_numWorkspaces < num; ++m_numWorkspaces)
        shell->addWorkspace();
    while (m_numWorkspaces > num)
        shell->removeWorkspace(--m_numWorkspaces);
}

#include "moc_shellui.cpp"
