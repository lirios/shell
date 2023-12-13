// SPDX-FileCopyrightText: 2022 Pier Luigi Fiorini <pierluigi.fiorini@gmail.com>
// SPDX-License-Identifier: GPL-3.0-or-later

#pragma once

#include <QWindow>
#include <QQmlParserStatus>

class ExtSessionLockSurfaceV1Private;

class ExtSessionLockSurfaceV1 : public QObject, public QQmlParserStatus
{
    Q_OBJECT
    Q_DECLARE_PRIVATE(ExtSessionLockSurfaceV1)
    Q_PROPERTY(QWindow *window READ window WRITE setWindow NOTIFY windowChanged)
    Q_INTERFACES(QQmlParserStatus)
public:
    ExtSessionLockSurfaceV1(QObject *parent = nullptr);
    ~ExtSessionLockSurfaceV1();

    bool isInitialized() const;

    void initialize();

    QWindow *window() const;
    void setWindow(QWindow *window);

    static ExtSessionLockSurfaceV1 *get(QWindow *window);

protected:
    void classBegin() override {}
    void componentComplete() override;

Q_SIGNALS:
    void windowChanged(QWindow *window);
    void unlockRequested();

private:
    QScopedPointer<ExtSessionLockSurfaceV1Private> const d_ptr;
};
