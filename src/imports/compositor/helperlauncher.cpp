// SPDX-FileCopyrightText: 2022 Pier Luigi Fiorini <pierluigi.fiorini@gmail.com>
//
// SPDX-License-Identifier: GPL-3.0-or-later

#include <QtCore/QFileInfo>

#include "helperlauncher.h"
#include "lirishellcompositorlogging.h"

HelperLauncher::HelperLauncher(QObject *parent)
    : QObject(parent)
    , m_crashTimer(new QTimer(this))
{
    m_crashTimer->setSingleShot(true);
    connect(m_crashTimer, &QTimer::timeout, this, &HelperLauncher::handleResetCrashCount);
}

HelperLauncher::~HelperLauncher()
{
    setRunning(false);
}

HelperLauncher::Helper HelperLauncher::helper() const
{
    return m_helper;
}

void HelperLauncher::setHelper(Helper helper)
{
    if (m_helper == helper)
        return;

    m_helper = helper;
    emit helperChanged();
}

bool HelperLauncher::isRunning() const
{
    return m_running;
}

void HelperLauncher::setRunning(bool running)
{
    if (m_running == running)
        return;

    m_running = running;
    emit runningChanged();

    if (m_initialized) {
        if (m_running)
            startProcess();
        else
            stopProcess();
    }
}

int HelperLauncher::maxCrashCount() const
{
    return m_maxCrashCount;
}

void HelperLauncher::setMaxCrashCount(int maxCrashCount)
{
    if (m_maxCrashCount == maxCrashCount)
        return;

    m_maxCrashCount = maxCrashCount;
    emit maxCrashCountChanged();
}

QByteArray HelperLauncher::socketName() const
{
    return m_socketName;
}

void HelperLauncher::setSocketName(const QByteArray &name)
{
    if (m_socketName == name)
        return;

    m_socketName = name;
    emit socketNameChanged();
}

void HelperLauncher::start()
{
    setRunning(true);
}

void HelperLauncher::stop()
{
    setRunning(false);
}

void HelperLauncher::componentComplete()
{
    if (m_running)
        startProcess();
    else
        stopProcess();

    m_initialized = true;
}

bool HelperLauncher::startProcess()
{
    QMutexLocker lock(&m_mutex);

    // Stop process if it was previously running
    stopProcess();

    QString cmd;

    switch (m_helper) {
    case Shell:
        cmd = QString::fromLocal8Bit(LIBEXECDIR "/liri-shell-helper");
        break;
    case SessionLocker:
        cmd = QString::fromLocal8Bit(LIBEXECDIR "/liri-shell-lockscreen");
        break;
    case Greeter:
        cmd = QString::fromLocal8Bit(LIBEXECDIR "/liri-shell-greeter");
        break;
    }

    auto args = QProcess::splitCommand(cmd);
    m_program = args.takeFirst();

    QFileInfo programInfo(m_program);
    m_baseName = programInfo.baseName();

    qCDebug(lcShellCompositor, "Running helper: %s", qPrintable(cmd));

    auto env = QProcessEnvironment::systemEnvironment();
    env.remove(QStringLiteral("DISPLAY"));
    env.remove(QStringLiteral("WAYLAND_DISPLAY"));
    env.remove(QStringLiteral("WAYLAND_SOCKET"));
    env.insert(QStringLiteral("XDG_SESSION_TYPE"), QStringLiteral("wayland"));
    env.insert(QStringLiteral("QT_QPA_PLATFORM"), QStringLiteral("wayland"));
    if (!m_socketName.isEmpty())
        env.insert(QStringLiteral("WAYLAND_DISPLAY"), QString::fromLocal8Bit(m_socketName));

    m_process = new QProcess(this);
    m_process->setProcessEnvironment(env);
    m_process->setProcessChannelMode(QProcess::SeparateChannels);

    connect(m_process, &QProcess::readyReadStandardError, this, [&] {
        const QByteArrayList lines = m_process->readAllStandardError().split('\n');
        for (const QByteArray &line : lines) {
            if (line.isEmpty())
                continue;

            qCWarning(lcShellCompositor, "[%s] %s", qPrintable(m_baseName), line.constData());
        }
    });
    connect(m_process, &QProcess::readyReadStandardOutput, this, [&] {
        const QByteArrayList lines = m_process->readAllStandardOutput().split('\n');
        for (const QByteArray &line : lines) {
            if (line.isEmpty())
                continue;

            qCInfo(lcShellCompositor, "[%s] %s", qPrintable(m_baseName), line.constData());
        }
    });
    connect(m_process, &QProcess::errorOccurred, this, [&](QProcess::ProcessError error) {
        switch (error) {
        case QProcess::FailedToStart:
            qCWarning(lcShellCompositor, "Failed to start \"%s\"", qPrintable(m_baseName));
            break;
        case QProcess::Crashed:
            qCWarning(lcShellCompositor, "Process \"%s\" crashed", qPrintable(m_baseName));
            break;
        case QProcess::Timedout:
            qCWarning(lcShellCompositor, "Operation timedout starting \"%s\"", qPrintable(m_baseName));
            break;
        case QProcess::ReadError:
        case QProcess::WriteError:
            qCWarning(lcShellCompositor, "An error has occurred while communicating with \"%s\"", qPrintable(m_baseName));
            break;
        case QProcess::UnknownError:
            qCWarning(lcShellCompositor, "Unknown error occurred with \"%s\"", qPrintable(m_baseName));
            break;
        }

        emit errorOccurred();
    });
    connect(m_process, QOverload<int, QProcess::ExitStatus>::of(&QProcess::finished),
            m_process, [&](int exitCode, QProcess::ExitStatus exitStatus) {
        m_crashTimer->stop();

        if (++m_crashCount <= 5) {
            if (m_process && m_process->state() != QProcess::NotRunning) {
                stopProcess();
                emit finished();
            }

            startProcess();

            m_crashTimer->start(std::chrono::minutes(10));
        }
    });

    m_process->start(m_program, args);
    if (!m_process->waitForStarted(10000)) {
        qCWarning(lcShellCompositor, "Failed to start \"%s\": %s",
                  qPrintable(cmd),
                  qPrintable(m_process->errorString()));
        cleanProcess();
        return false;
    }

    emit started();

    return true;
}

void HelperLauncher::stopProcess()
{
    if (m_process) {
        disconnect(m_process, nullptr, this, nullptr);
        m_process->terminate();
        if (!m_process->waitForFinished(5000))
            m_process->kill();
        m_process->deleteLater();
        m_process = nullptr;
    }
}

void HelperLauncher::cleanProcess()
{
    if (m_process) {
        disconnect(m_process, nullptr, this, nullptr);
        m_process->deleteLater();
        m_process = nullptr;
    }
}

void HelperLauncher::handleResetCrashCount()
{
    m_crashCount = 0;
}
