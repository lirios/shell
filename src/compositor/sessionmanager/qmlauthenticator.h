// SPDX-FileCopyrightText: 2018 Pier Luigi Fiorini <pierluigi.fiorini@gmail.com>
//
// SPDX-License-Identifier: GPL-3.0-or-later

#pragma once

#include <QObject>
#include <QJSValue>

#include "sessionmanager/sessionmanager.h"

class QmlAuthenticator : public QObject
{
    Q_OBJECT
public:
    explicit QmlAuthenticator(SessionManager *parent, const QJSValue &callback);

private:
    SessionManager *m_parent;
    QJSValue m_callback;
    bool m_succeded;

private Q_SLOTS:
    void authenticate();
};
