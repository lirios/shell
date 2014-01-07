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
#include <QtQml/QQmlEngine>

#include <QtGui/qpa/qplatformnativeinterface.h>

#include "desktopview.h"
#include "grabwindow.h"
#include "lockscreenwindow.h"
#include "panelview.h"
#include "shellui.h"
#include "shellmanager.h"

const QString s_desktopViewFileName = QStringLiteral("views/DesktopView.qml");

ShellUi::ShellUi(QObject *parent)
    : QObject(parent)
    , m_numWorkspaces(0)
    , m_lockScreenWindow(nullptr)
{
    // Create and connect JavaScript engine
    m_jsEngine = new ScriptEngine(this);
    connect(m_jsEngine, &ScriptEngine::print,
            this, &ShellUi::printScriptMessage);
    connect(m_jsEngine, &ScriptEngine::printError,
            this, &ShellUi::printScriptError);

    // Create grab window
    m_grabWindow = new GrabWindow();
    m_grabWindow->show();
}

ShellUi::~ShellUi()
{
    if (m_grabWindow)
        m_grabWindow->deleteLater();
    if (m_lockScreenWindow)
        m_lockScreenWindow->deleteLater();
    qDeleteAll(m_panelViews);
    qDeleteAll(m_desktopViews);
}

ScriptEngine *ShellUi::jsEngine() const
{
    return m_jsEngine;
}

GrabWindow *ShellUi::grabWindow() const
{
    return m_grabWindow;
}

LockScreenWindow *ShellUi::lockScreenWindow() const
{
    return m_lockScreenWindow;
}

QList<DesktopView *> ShellUi::desktops() const
{
    return m_desktopViews;
}

QList<PanelView *> ShellUi::panels() const
{
    return m_panelViews;
}

void ShellUi::load()
{
    for (QScreen *screen: QGuiApplication::screens())
        screenAdded(screen);
    connect(qApp, &QGuiApplication::screenAdded,
            this, &ShellUi::screenAdded);

    QString fileName = ShellManager::instance()->shellDirectory().filePath("layout.js");
    QFile file(fileName);
    if (file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        QString script = file.readAll();
        file.close();
        m_jsEngine->evaluateScript(script, "layout.js");
    }
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

void ShellUi::setGrabCursor(const QCursor &cursor)
{
    m_grabWindow->setGrabCursor(cursor);
}

void ShellUi::setNumWorkspaces(int num)
{
    // At least one!
    if (num <= 0) {
        qWarning() << "Setting" << num << "workspaces doesn't make any sense, please set at least one";
        return;
    }

    ShellController *controller = ShellManager::instance()->controller();

    // Add as many workspaces as needed
    for (; m_numWorkspaces < num; ++m_numWorkspaces)
        controller->addWorkspace();
    while (m_numWorkspaces > num)
        controller->removeWorkspace(--m_numWorkspaces);
}

void ShellUi::screenAdded(QScreen *screen)
{
    QQmlEngine *engine = ShellManager::instance()->engine();
    QDir shellDir = ShellManager::instance()->shellDirectory();

    DesktopView *view = new DesktopView(engine, screen);
    view->setSource(QUrl::fromLocalFile(shellDir.absoluteFilePath(s_desktopViewFileName)));
    view->show();
    m_desktopViews.append(view);

    connect(screen, &QObject::destroyed,
            this, &ShellUi::screenDestroyed);
}

void ShellUi::screenDestroyed(QObject *object)
{
    QScreen *screen = qobject_cast<QScreen *>(object);
    if (!screen)
        return;

    // Remove desktop view for this screen
    for (DesktopView *view: m_desktopViews) {
        if (view->screen() == screen) {
            view->deleteLater();
            m_desktopViews.removeOne(view);
        }
    }

    // Move all panels on a deleted screen to the primary screen
    for (PanelView *view: m_panelViews)
        view->setScreen(QGuiApplication::primaryScreen());
}

void ShellUi::printScriptMessage(const QString &text)
{
    qDebug() << qPrintable(text);
}

void ShellUi::printScriptError(const QString &text)
{
    qWarning() << qPrintable(text);
}

#include "moc_shellui.cpp"
