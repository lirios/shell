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
#include <QtGui/qpa/qplatformnativeinterface.h>
#include <QtQml/QQmlEngine>

#include <HawaiiShell/PluginLoader>

#include "applicationiconprovider.h"
#include "backgroundview.h"
#include "desktopview.h"
#include "grabwindow.h"
#include "lockscreenview.h"
#include "panelview.h"
#include "shellui.h"
#include "shellmanager.h"

const QString s_lockScreenViewFileName = QStringLiteral("lockscreen/LockScreen.qml");

ShellUi::ShellUi(QObject *parent)
    : Hawaii::Shell::Corona(parent)
    , m_numWorkspaces(0)
    , m_grabWindow(nullptr)
    , m_lockScreenView(nullptr)
{
    // Register image provider
    engine()->addImageProvider("appicon", new ApplicationIconProvider);

    // Create and connect JavaScript engine
    m_jsEngine = new ScriptEngine(this);
    connect(m_jsEngine, &ScriptEngine::print,
            this, &ShellUi::printScriptMessage);
    connect(m_jsEngine, &ScriptEngine::printError,
            this, &ShellUi::printScriptError);
}

ShellUi::~ShellUi()
{
    if (m_grabWindow)
        m_grabWindow->deleteLater();
    if (m_lockScreenView)
        m_lockScreenView->deleteLater();
    qDeleteAll(m_panelViews);
    qDeleteAll(m_desktopViews);
    qDeleteAll(m_backgroundViews);
}

ScriptEngine *ShellUi::jsEngine() const
{
    return m_jsEngine;
}

GrabWindow *ShellUi::grabWindow() const
{
    return m_grabWindow;
}

LockScreenView *ShellUi::lockScreenView() const
{
    return m_lockScreenView;
}

QList<BackgroundView *> ShellUi::backgrounds() const
{
    return m_backgroundViews;
}

QList<DesktopView *> ShellUi::desktops() const
{
    return m_desktopViews;
}

QList<PanelView *> ShellUi::panels() const
{
    return m_panelViews;
}

void ShellUi::addPanelView(PanelView *view)
{
    m_panelViews.append(view);
}

void ShellUi::load()
{
    // Create grab window
    m_grabWindow = new GrabWindow();
    m_grabWindow->show();

    // Create new desktop views when screens are attached
    connect(qApp, &QGuiApplication::screenAdded,
            this, &ShellUi::screenAdded);

    // Create desktop view for each screen
    for (QScreen *screen: QGuiApplication::screens())
        screenAdded(screen);

    // Execute layout script
    QFile file(package().filePath("defaultlayout"));
    if (file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        QString script = file.readAll();
        file.close();
        m_jsEngine->evaluateScript(script, file.fileName());
    }
}

void ShellUi::unload()
{
    // Don't unload until we know which shell is loaded
    if (shell().isEmpty())
        return;

    // Destroy grab window
    if (m_grabWindow) {
        m_grabWindow->deleteLater();
        m_grabWindow = nullptr;
    }

    // Destroy all panels
    for (PanelView *view: m_panelViews) {
        m_panelViews.removeOne(view);
        view->deleteLater();
    }
}

void ShellUi::createLockScreen()
{
    if (!m_lockScreenView) {
        QDir dir = ShellManager::instance()->lookAndFeelDirectory();

        m_lockScreenView = new LockScreenView(this, QGuiApplication::primaryScreen());
        m_lockScreenView->setSource(QUrl::fromLocalFile(dir.absoluteFilePath(s_lockScreenViewFileName)));
    }
    m_lockScreenView->show();
}

void ShellUi::closeLockScreen()
{
    if (!m_lockScreenView)
        return;

    m_lockScreenView->deleteLater();
    m_lockScreenView = nullptr;
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

void ShellUi::changeShell(const QString &value)
{
    // Avoid loading the same shell twice
    if (shell() == value)
        return;

    // Keep track of elapsed time
    QElapsedTimer elapsed;
    elapsed.start();

    // Unload the old shell
    unload();

    // Save shell name
    setShell(value);

    // Load package
    Hawaii::Shell::Package package = Hawaii::Shell::PluginLoader::instance()->loadPackage("Hawaii/Shell/Shell");
    package.setPath(value);
    setPackage(package);

    // Load the new one
    load();

    // Print how much did it take to load the new shell
    qDebug() << "Shell handler" << shell() << "loaded in" << elapsed.elapsed() << "ms";
}

void ShellUi::setLookAndFeel(const QString &lookAndFeel)
{
    // Avoid loading the same look and feel package twice
    if (m_lookAndFeel == lookAndFeel)
        return;

    // Save look and feel
    m_lookAndFeel = lookAndFeel;

    // Change user interface
    if (m_lockScreenView) {
        QDir dir = ShellManager::instance()->lookAndFeelDirectory();
        m_lockScreenView->setSource(QUrl::fromLocalFile(dir.absoluteFilePath(s_lockScreenViewFileName)));
    }
}

void ShellUi::synchronize()
{
    while (QCoreApplication::hasPendingEvents())
        QCoreApplication::processEvents();
}

void ShellUi::screenAdded(QScreen *screen)
{
    BackgroundView *background = nullptr;
    DesktopView *desktop = nullptr;
    bool backgroundPreviouslyAdded = false;
    bool desktopPreviouslyAdded = false;

    // Create views only when necessary
    for (BackgroundView *currentView: m_backgroundViews) {
        if (currentView->screen() == screen) {
            background = currentView;
            backgroundPreviouslyAdded = true;
            break;
        }
    }
    for (DesktopView *currentView: m_desktopViews) {
        if (currentView->screen() == screen) {
            desktop = currentView;
            desktopPreviouslyAdded = true;
            break;
        }
    }

    // Load background QML code and show
    if (!background)
        background = new BackgroundView(screen);
    background->loadPlugin();
    if (!backgroundPreviouslyAdded) {
        background->show();
        m_backgroundViews.append(background);
    }

    // Load desktop QML code and show
    if (!desktop)
        desktop = new DesktopView(this, screen);
    desktop->setSource(QUrl::fromLocalFile(package().filePath("views", QStringLiteral("DesktopView.qml"))));
    if (!desktopPreviouslyAdded) {
        desktop->show();
        m_desktopViews.append(desktop);
    }

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

    // Remove background view for this screen
    for (BackgroundView *view: m_backgroundViews) {
        if (view->screen() == screen) {
            view->deleteLater();
            m_backgroundViews.removeOne(view);
        }
    }
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
