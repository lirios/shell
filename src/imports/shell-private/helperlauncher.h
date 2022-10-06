// SPDX-FileCopyrightText: 2022 Pier Luigi Fiorini <pierluigi.fiorini@gmail.com>
//
// SPDX-License-Identifier: GPL-3.0-or-later

#ifndef HELPERLAUNCHER_H
#define HELPERLAUNCHER_H

#include <QtCore/QMap>
#include <QtCore/QMutex>
#include <QtCore/QProcess>
#include <QtCore/QTimer>
#include <QtQml/QQmlParserStatus>

class HelperLauncher : public QObject, public QQmlParserStatus
{
    Q_OBJECT
    Q_PROPERTY(Helper helper READ helper WRITE setHelper NOTIFY helperChanged)
    Q_PROPERTY(bool running READ isRunning WRITE setRunning NOTIFY runningChanged)
    Q_PROPERTY(int maxCrashCount READ maxCrashCount WRITE setMaxCrashCount NOTIFY maxCrashCountChanged)
    Q_PROPERTY(QByteArray socketName READ socketName WRITE setSocketName NOTIFY socketNameChanged)
    Q_INTERFACES(QQmlParserStatus)
public:
    enum Helper {
        Shell,
        SessionLocker
    };
    Q_ENUM(Helper)

    HelperLauncher(QObject *parent = nullptr);
    ~HelperLauncher();

    Helper helper() const;
    void setHelper(Helper helper);

    bool isRunning() const;
    void setRunning(bool running);

    int maxCrashCount() const;
    void setMaxCrashCount(int maxCrashCount);

    QByteArray socketName() const;
    void setSocketName(const QByteArray &name);

    Q_INVOKABLE void start();
    Q_INVOKABLE void stop();

    void classBegin() override {}
    void componentComplete() override;

Q_SIGNALS:
    void helperChanged();
    void runningChanged();
    void maxCrashCountChanged();
    void socketNameChanged();
    void started();
    void finished();
    void errorOccurred();

private:
    Helper m_helper = Shell;
    bool m_running = false;
    bool m_initialized = false;
    QString m_program;
    QString m_baseName;
    QProcess *m_process = nullptr;

    int m_maxCrashCount = 5;
    int m_crashCount = 0;
    QTimer *m_crashTimer = nullptr;

    QByteArray m_socketName;

    QMutex m_mutex;

    bool startProcess();
    void stopProcess();
    void cleanProcess();

private Q_SLOTS:
    void handleResetCrashCount();
};

#endif // HELPERLAUNCHER_H
