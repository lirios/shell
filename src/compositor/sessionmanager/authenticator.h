// SPDX-FileCopyrightText: 2018 Pier Luigi Fiorini
//
// SPDX-License-Identifier: GPL-3.0-or-later

#ifndef AUTHENTICATOR_H
#define AUTHENTICATOR_H

#include <QtCore/QObject>

struct pam_message;
struct pam_response;

class Authenticator : public QObject
{
    Q_OBJECT
public:
    Authenticator(QObject *parent = 0);
    ~Authenticator();

public Q_SLOTS:
    void authenticate(const QString &password);

Q_SIGNALS:
    void authenticationSucceded();
    void authenticationFailed();
    void authenticationError();

private:
    pam_response *m_response;

    static int conversationHandler(int num, const pam_message **message,
                                   pam_response **response, void *data);
};

#endif // AUTHENTICATOR_H
