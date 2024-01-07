// SPDX-FileCopyrightText: 2018-2021 Pier Luigi Fiorini <pierluigi.fiorini@gmail.com>
//
// SPDX-License-Identifier: GPL-3.0-or-later

#ifndef AUTHENTICATOR_H
#define AUTHENTICATOR_H

#include <QObject>
#include <QJSValue>
#include <QQmlEngine>

extern "C" {
struct pam_message;
struct pam_response;
}

class Authenticator : public QObject
{
    Q_OBJECT
    QML_ELEMENT
    QML_SINGLETON
public:
    Authenticator(QObject *parent = nullptr);
    ~Authenticator();

    Q_INVOKABLE void authenticate(const QString &password, const QJSValue &callback);

    static Authenticator *create(QQmlEngine *qmlEngine, QJSEngine *jsEngine);

public Q_SLOTS:

Q_SIGNALS:
    void authenticationSucceded();
    void authenticationFailed();
    void authenticationError();

private:
    bool m_authRequested = false;
    QJSValue m_callback;
    pam_response *m_response = nullptr;

    void actualAuthentication(const QString &password);
    void sendReply(bool succeeded);

    static int conversationHandler(int num, const pam_message **message,
                                   pam_response **response, void *data);
};

#endif // AUTHENTICATOR_H
