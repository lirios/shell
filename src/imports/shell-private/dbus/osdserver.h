// SPDX-FileCopyrightText: 2019 Pier Luigi Fiorini <pierluigi.fiorini@gmail.com>
//
// SPDX-License-Identifier: GPL-3.0-or-later

#ifndef OSDSERVER_H
#define OSDSERVER_H

#include <QObject>

class OsdServer : public QObject
{
    Q_OBJECT
    Q_CLASSINFO("D-Bus Interface", "io.liri.Shell.Osd")
public:
    explicit OsdServer(QObject *parent = nullptr);

public Q_SLOTS:
    void ShowText(const QString &iconName, const QString &text = QString());
    void ShowProgress(const QString &iconName, int value);

Q_SIGNALS:
    void textRequested(const QString &iconName, const QString &text);
    void progressRequested(const QString &iconName, int value);
};

#endif // OSDSERVER_H
