/****************************************************************************
 * This file is part of Hawaii Shell.
 *
 * Copyright (C) 2014 Pier Luigi Fiorini <pierluigi.fiorini@gmail.com>
 *
 * Author(s):
 *    Pier Luigi Fiorini
 *
 * $BEGIN_LICENSE:LGPL2.1+$
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License as published by
 * the Free Software Foundation, either version 2.1 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 *
 * $END_LICENSE$
 ***************************************************************************/

#ifndef SHELLMANAGER_H
#define SHELLMANAGER_H

#include <QtCore/QDir>
#include <QtCore/QElapsedTimer>
#include <QtCore/QObject>
#include <QtCore/QHash>
#include <QtCore/QTimer>

#include <HawaiiShell/ShellSettings>

#include "registrylistener.h"
#include "shellclient.h"
#include "shellcontroller.h"
#include "shellsurfaceclient.h"
#include "shellui.h"

class QQmlEngine;

class ShellManager : public QObject
{
    Q_OBJECT
    Q_PROPERTY(QString shell READ shell NOTIFY shellChanged)
    Q_PROPERTY(QString lookAndFeel READ lookAndFeel NOTIFY lookAndFeelChanged)
public:
    ShellManager();
    virtual ~ShellManager();

    static ShellManager *instance();

    ShellController *controller() const;
    ShellUi *corona() const;

    ShellClient *shellInterface() const;
    ShellSurfaceClient *shellSurfaceInterface() const;

    QString shell() const;

    QString lookAndFeel() const;
    QDir lookAndFeelDirectory() const;

    void loadHandlers();
    void loadLookAndFeel();

public Q_SLOTS:
    void setup();
    void create();

Q_SIGNALS:
    void ready();
    void shellChanged(const QString &shell);
    void lookAndFeelChanged(const QString &lookAndFeel);

private:
    QElapsedTimer m_elapsedTimer;
    QTimer m_updateTimer;
    ShellSettings *m_settings;
    RegistryListener *m_registryListener;
    ShellController *m_shellController;
    ShellUi *m_shellUi;
    QHash<QString, QObject *> m_handlers;
    QObject *m_currentHandler;
    QDir m_lookAndFeelDir;

private Q_SLOTS:
    void registerHandler(const QString &name, QObject *handler);
    void deregisterHandler(QObject *handler);

    void requestShellUpdate();
    void updateShell();
};

#endif // SHELLMANAGER_H
