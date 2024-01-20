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

static int convertPermission(const QFileInfo &fileInfo)
{
    int p = 0;

    if (fileInfo.permission(QFile::ReadUser))
        p += 400;
    if (fileInfo.permission(QFile::WriteUser))
        p += 200;
    if (fileInfo.permission(QFile::ExeUser))
        p += 100;
    if (fileInfo.permission(QFile::ReadGroup))
        p += 40;
    if (fileInfo.permission(QFile::WriteGroup))
        p += 20;
    if (fileInfo.permission(QFile::ExeGroup))
        p += 10;
    if (fileInfo.permission(QFile::ReadOther))
        p += 4;
    if (fileInfo.permission(QFile::WriteOther))
        p += 2;
    if (fileInfo.permission(QFile::ExeOther))
        p += 1;

    return p;
}

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

void Application::verifyXdgRuntimeDir()
{
    QString dirName = QString::fromLocal8Bit(qgetenv("XDG_RUNTIME_DIR"));

    if (dirName.isEmpty()) {
        QString msg = QObject::tr(
                    "The XDG_RUNTIME_DIR environment variable is not set.\n"
                    "Refer to your distribution on how to get it, or read\n"
                    "http://www.freedesktop.org/wiki/Specifications/basedir-spec\n"
                    "on how to implement it.\n");
        qFatal("%s", qPrintable(msg));
    }

    QFileInfo fileInfo(dirName);

    if (!fileInfo.exists()) {
        QString msg = QObject::tr(
                    "The XDG_RUNTIME_DIR environment variable is set to "
                    "\"%1\", which doesn't exist.\n").arg(dirName.constData());
        qFatal("%s", qPrintable(msg));
    }

    if (convertPermission(fileInfo) != 700 || fileInfo.ownerId() != getuid()) {
        QString msg = QObject::tr(
                    "XDG_RUNTIME_DIR is set to \"%1\" and is not configured correctly.\n"
                    "Unix access mode must be 0700, but is 0%2.\n"
                    "It must also be owned by the current user (UID %3), "
                    "but is owned by UID %4 (\"%5\").\n")
                .arg(dirName.constData())
                .arg(convertPermission(fileInfo))
                .arg(getuid())
                .arg(fileInfo.ownerId())
                .arg(fileInfo.owner());
        qFatal("%s\n", qPrintable(msg));
    }
}

void Application::startup()
{
    // Can't do the startup sequence twice
    if (m_started)
        return;

    // Check whether XDG_RUNTIME_DIR is ok or not
    verifyXdgRuntimeDir();

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
