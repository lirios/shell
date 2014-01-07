/****************************************************************************
 * This file is part of Hawaii Shell.
 *
 * Copyright (C) 2014 Pier Luigi Fiorini <pierluigi.fiorini@gmail.com>
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
#include <QtCore/QDir>
#include <QtGui/QGuiApplication>
#include <QtQml/QQmlComponent>
#include <QtQml/QQmlContext>
#include <QtQml/QQmlEngine>
#include <QtQuick/QQuickWindow>

#include <HawaiiShell/PluginMetadata>

#include "applicationiconprovider.h"
#include "cmakedirs.h"
#include "registration.h"
#include "shellmanager.h"

Q_GLOBAL_STATIC(ShellManager, s_shellManager)

ShellManager::ShellManager()
    : m_engine(new QQmlEngine(this))
    , m_registryListener(new RegistryListener())
    , m_shellController(nullptr)
    , m_shellUi(nullptr)
    , m_currentHandler(nullptr)
{
    // Start counting how much time we need to start up :)
    m_elapsedTimer.start();

    // We need windows with alpha buffer
    QQuickWindow::setDefaultAlphaBuffer(true);

    // Register image provider
    m_engine->addImageProvider("appicon", new ApplicationIconProvider);

    // Register QML types
    Registration::registerQmlTypes();

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

QQmlEngine *ShellManager::engine() const
{
    return m_engine;
}

ShellController *ShellManager::controller() const
{
    return m_shellController;
}

ShellUi *ShellManager::ui() const
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

QString ShellManager::shell() const
{
    if (m_currentHandler)
        return m_currentHandler->property("shell").toString();
    return QString();
}

QDir ShellManager::shellDirectory() const
{
    if (m_currentHandler)
        return m_currentHandler->property("path").toString();
    return QDir();
}

void ShellManager::loadHandlers()
{
    QDir shellsDir(QStringLiteral(INSTALL_DATADIR) + QStringLiteral("/hawaii/shells"));
    for (const QString &dirName: shellsDir.entryList(QDir::Dirs | QDir::NoDotAndDotDot)) {
        const QString &metadataFileName = shellsDir.absoluteFilePath(dirName + QStringLiteral("/metadata.desktop"));
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
        QQmlComponent component(m_engine, qmlFileName, this);
        QObject *handler = component.create();
        for (QQmlError error: component.errors())
            qWarning() << "Error:" << error.toString();

        if (handler) {
            // Register shell handler and save the base directory
            registerHandler(name, handler);

            // Save shell path
            const QString path = shellsDir.absoluteFilePath(dirName + QStringLiteral("/contents/"));
            handler->setProperty("path", path);;
        }
    }

    updateShell();
}

void ShellManager::setup()
{
    // Load shell handlers
    loadHandlers();

    // Create the shell controller
    m_shellController = new ShellController(this);
    m_engine->rootContext()->setContextProperty("Shell", m_shellController);
}

void ShellManager::create()
{
    // Create shell user interface
    m_shellUi = new ShellUi(this);
    m_engine->rootContext()->setContextProperty("Ui", m_shellUi);
    connect(m_registryListener->shell, &ShellClient::prepareLockSurface,
            m_shellUi, &ShellUi::createLockScreenWindow);
    connect(m_registryListener->shell, &ShellClient::cursorChanged,
            m_shellUi, &ShellUi::setGrabCursor);

    // Add configured workspaces
    // TODO: Add as many workspaces as specified by the settings
    m_shellController->addWorkspaces(4);

    // Load user interface
    m_shellUi->load();

#if 0
    // Register daemons and singletons
    d->registrar = new QQmlComponent(d->engine, QUrl("qrc:/qml/Registrar.qml"), this);
    if (d->registrar->status() != QQmlComponent::Ready)
        qFatal("Unable to register daemons and singletons: %s",
               qPrintable(d->registrar->errorString()));
    (void)d->registrar->create();
#endif

    // Wait until all user interface elements for all screens are ready
    while (QCoreApplication::hasPendingEvents())
        QCoreApplication::processEvents();

    // Shell user interface is ready, tell the compositor to fade in
    qDebug() << "Shell is now ready, elapsed time:" << m_elapsedTimer.elapsed() << "ms";
    Q_EMIT ready();
}

void ShellManager::updateShell()
{
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
    Q_EMIT shellChanged(m_currentHandler->property("shell").toString());
}

void ShellManager::registerHandler(const QString &name, QObject *handler)
{
    connect(handler, &QObject::destroyed,
            this, &ShellManager::deregisterHandler);

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
