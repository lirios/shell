// SPDX-FileCopyrightText: 2018 Pier Luigi Fiorini
//
// SPDX-License-Identifier: GPL-3.0-or-later

#ifndef SESSIONMANAGER_H
#define SESSIONMANAGER_H

#include <QtCore/QObject>
#include <QtCore/QLoggingCategory>

Q_DECLARE_LOGGING_CATEGORY(SESSION_MANAGER)

class SessionManager : public QObject
{
    Q_OBJECT
public:
    SessionManager(QObject *parent = nullptr);
    virtual ~SessionManager();

public Q_SLOTS:
    void registerService();
};

#endif // SESSIONMANAGER_H
