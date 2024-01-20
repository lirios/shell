// SPDX-FileCopyrightText: 2018 Pier Luigi Fiorini
//
// SPDX-License-Identifier: GPL-3.0-or-later

#include <QCoreApplication>
#include <QDir>
#include <QDBusConnection>
#include <QDBusError>
#include <QQmlApplicationEngine>
#include <QQmlContext>
#include <QQmlExtensionPlugin>
#include <QStandardPaths>
#include <QtPlugin>

#include "application.h"
#include "sessionmanager/sessionmanager.h"

#include <unistd.h>
#include <sys/types.h>

static const QEvent::Type StartupEventType = static_cast<QEvent::Type>(QEvent::registerEventType());

Application::Application(QObject *parent)
    : QObject(parent)
    , m_failSafe(false)
    , m_started(false)
{
    // Application engine
    m_appEngine = new QQmlApplicationEngine(this);
    connect(m_appEngine, &QQmlApplicationEngine::objectCreated,
            this, &Application::objectCreated);

    // Session manager
    m_sessionManager = new SessionManager(this);

    // Invoke shutdown sequence when quitting
    connect(QCoreApplication::instance(), &QCoreApplication::aboutToQuit,
            this, &Application::shutdown);
}

void Application::setUrl(const QUrl &url)
{
    m_url = url;
}

void Application::customEvent(QEvent *event)
{
    if (event->type() == StartupEventType)
        startup();
}

void Application::startup()
{
    // Can't do the startup sequence twice
    if (m_started)
        return;

    // Session interface
    m_appEngine->rootContext()->setContextProperty(QStringLiteral("SessionInterface"),
                                                   m_sessionManager);

    // Load the compositor
    m_appEngine->load(m_url);

    m_started = true;
}

void Application::shutdown()
{
    m_appEngine->deleteLater();
    m_appEngine = nullptr;

    m_sessionManager->deleteLater();
    m_sessionManager = nullptr;
}

void Application::objectCreated(QObject *object, const QUrl &)
{
    // All went fine
    if (object)
        return;

    // Compositor failed to load
    if (m_failSafe) {
        // We give up because even the error screen has an error
        qWarning("A fatal error has occurred while running Liri Shell, but the error "
                 "screen has errors too. Giving up.");
        QCoreApplication::exit(1);
    } else {
        // Load the error screen in case of error
        m_failSafe = true;
        m_appEngine->load(QUrl(QStringLiteral("qrc:/qt/qml/compositor/qml/error/ErrorCompositor.qml")));
    }
}

StartupEvent::StartupEvent()
    : QEvent(StartupEventType)
{
}

#include "moc_application.cpp"
