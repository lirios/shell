/****************************************************************************
 * This file is part of Hawaii Shell.
 *
 * Copyright (C) 2014 Pier Luigi Fiorini <pierluigi.fiorini@gmail.com>
 *
 * Author(s):
 *    Pier Luigi Fiorini
 *
 * $BEGIN_LICENSE:GPL2+$
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 *
 * $END_LICENSE$
 ***************************************************************************/

#include <QtCore/QDebug>
#include <QtCore/QDir>
#include <QtGui/QGuiApplication>
#include <QtQml/QQmlComponent>
#include <QtQml/QQmlContext>
#include <QtQml/QQmlEngine>
#include <QtQuick/QQuickWindow>

#include <HawaiiShell/PluginMetadata>

#include "cmakedirs.h"
#include "keybinding.h"
#include "keymap.h"
#include "registration.h"
#include "shellmanager.h"

Q_GLOBAL_STATIC(ShellManager, s_shellManager)

ShellManager::ShellManager()
    : m_settings(new QStaticConfiguration(this))
    , m_registryListener(new RegistryListener())
    , m_shellController(nullptr)
    , m_currentHandler(nullptr)
{
    // Start counting how much time we need to start up :)
    m_elapsedTimer.start();

    // Wait during shell switch
    m_updateTimer.setInterval(100);
    m_updateTimer.setSingleShot(true);
    connect(&m_updateTimer, &QTimer::timeout,
            this, &ShellManager::updateShell);

    // Settings
    m_settings->setCategory(QStringLiteral("shell"));

    // We need windows with alpha buffer
    QQuickWindow::setDefaultAlphaBuffer(true);

    // Register QML types and services
    Registration::registerQmlTypes();
    Registration::registerFactories();

    // Create shell mantle
    m_shellUi = new ShellUi(this);

    // Create UI when the compositor says so
    connect(m_registryListener->shell, &ShellClient::loaded,
            this, &ShellManager::create);

    // Register Wayland interfaces
    m_registryListener->run();
}

ShellManager::~ShellManager()
{
    delete m_registryListener;
}

ShellManager *ShellManager::instance()
{
    return s_shellManager();
}

ShellController *ShellManager::controller() const
{
    return m_shellController;
}

ShellUi *ShellManager::mantle() const
{
    return m_shellUi;
}

ShellClient *ShellManager::shellInterface() const
{
    return m_registryListener->shell;
}

ShellSurfaceClient *ShellManager::shellSurfaceInterface() const
{
    return m_registryListener->shellSurface;
}

QtWayland::wl_hawaii_panel_manager *ShellManager::panelManagerInterface() const
{
    return m_registryListener->panelManager;
}

QString ShellManager::shell() const
{
    if (m_currentHandler)
        return m_currentHandler->property("shell").toString();
    return QString();
}

QString ShellManager::lookAndFeel() const
{
    QVariant defaultValue = QStringLiteral("org.hawaii.lookandfeel.standard");
    return m_settings->value(QStringLiteral("lookAndFeel"), defaultValue).toString();
}

QDir ShellManager::lookAndFeelDirectory() const
{
    return m_lookAndFeelDir;
}

void ShellManager::loadHandlers()
{
    QDir shellsDir(QStringLiteral(INSTALL_DATADIR) + QStringLiteral("/hawaii/shells"));
    for (const QString &dirName: shellsDir.entryList(QDir::Dirs | QDir::NoDotAndDotDot)) {
        const QString &metadataFileName = shellsDir.absoluteFilePath(dirName + QStringLiteral("/metadata.json"));
        const QString &qmlFileName = shellsDir.absoluteFilePath(dirName + QStringLiteral("/contents/loader.qml"));

        // Try to load plugin metadata, return if it's not valid
        Hawaii::Shell::PluginMetadata metadata(metadataFileName);
        if (!metadata.isValid())
            continue;

        // Check whether we already added this shell
        const QString &name = metadata.internalName();
        if (m_handlers.contains(name))
            continue;

        // Load shell handler
        qDebug() << "Loading shell handler" << name;
        QQmlComponent component(m_shellUi->engine(), qmlFileName, this);
        QObject *handler = component.create();
        for (QQmlError error: component.errors())
            qWarning() << "Error:" << error.toString();

        // Register shell handler
        if (handler)
            registerHandler(name, handler);;
    }

    updateShell();
}

void ShellManager::loadLookAndFeel()
{
    // This is bad but it's acceptable since we currently have only one look & feel package,
    // a better loading mechanism will be implemented as soon as we have the concept of
    // packages and plugin loader
    m_lookAndFeelDir = QDir(QStringLiteral(INSTALL_DATADIR) + QStringLiteral("/hawaii/lookandfeel/org.hawaii.lookandfeel.standard/contents"));
}

void ShellManager::create()
{
    // Load shell handlers
    loadHandlers();

    // Load look & feel
    loadLookAndFeel();

    // Create the shell controller
    m_shellController = new ShellController(this);

    // Setup shell mantle
    m_shellUi->engine()->rootContext()->setContextProperty("Shell", m_shellController);
    m_shellUi->engine()->rootContext()->setContextProperty("Ui", m_shellUi);
    connect(m_registryListener->shell, &ShellClient::prepareLockSurface,
            m_shellUi, &ShellUi::createLockScreen);
    connect(m_registryListener->shell, &ShellClient::cursorChanged,
            m_shellUi, &ShellUi::setGrabCursor);
    connect(this, &ShellManager::shellChanged,
            m_shellUi, &ShellUi::changeShell);

    // Bind Meta-D to toggle windows
    KeyBinding *binding = m_shellController->addKeyBinding(KEY_D, MODIFIER_SUPER);
    connect(binding, &KeyBinding::triggered, [=]() {
        m_shellController->toggleWindows();
    });

    // Add configured workspaces
    // TODO: Add as many workspaces as specified by the settings
    m_shellController->addWorkspaces(4);

    // Load user interface
    m_shellUi->changeShell(m_currentHandler->property("shell").toString());
    m_shellUi->setLookAndFeel("org.hawaii.lookandfeel.standard");

    // Shell user interface is ready, tell the compositor to fade in
    qDebug() << "Shell is now ready, elapsed time:" << m_elapsedTimer.elapsed() << "ms";
    Q_EMIT ready();
}

void ShellManager::requestShellUpdate()
{
    m_updateTimer.start();
}

void ShellManager::updateShell()
{
    m_updateTimer.stop();

    if (m_handlers.isEmpty()) {
        qFatal("No shell handlers installed, cannot continue!");
        return;
    }

    auto compare = [](QObject *left, QObject *right) {
        // A couple of functions to query for handler properties
        auto willing = [](QObject *handler) {
            return handler->property("willing").toBool();
        };
        auto priority = [](QObject *handler) {
            return handler->property("priority").toInt();
        };

        // If a handler is willing (for example it might be more suitable
        // for the current input devices set) and the other is not just
        // return it otherwise compare priorities
        return willing(left) && !willing(right)
                ? true
                : !willing(left) && willing(right)
                  ? false
                  : priority(left) < priority(right);
    };

    // Let's find out which handler we are going to use
    QList<QObject *> objects = m_handlers.values();
    QObject *handler = *std::min_element(objects.begin(), objects.end(), compare);

    // No point if it's the same
    if (m_currentHandler == handler)
        return;

    // Unload the old one and load the new one
    if (m_currentHandler)
        m_currentHandler->setProperty("loaded", false);
    m_currentHandler = handler;
    m_currentHandler->setProperty("loaded", true);

    // Tell listeners that we changed the shell handler
    const QString currentShell = m_currentHandler->property("shell").toString();
    qDebug() << "-> Current shell handler is" << currentShell;
    Q_EMIT shellChanged(currentShell);
}

void ShellManager::registerHandler(const QString &name, QObject *handler)
{
    connect(handler, &QObject::destroyed,
            this, &ShellManager::deregisterHandler);
    connect(handler, SIGNAL(willingChanged()),
            this, SLOT(requestShellUpdate()));
    connect(handler, SIGNAL(priorityChanged()),
            this, SLOT(requestShellUpdate()));

    m_handlers.insert(name, handler);
}

void ShellManager::deregisterHandler(QObject *handler)
{
    // Remove handler and disconnect all signals
    m_handlers.remove(m_handlers.key(handler));
    handler->disconnect(this);

    // If this is the current handler we update the shell
    if (m_currentHandler == handler) {
        m_currentHandler = nullptr;
        updateShell();
    }
}

#include "moc_shellmanager.cpp"
