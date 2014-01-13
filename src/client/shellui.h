/****************************************************************************
 * This file is part of Hawaii Shell.
 *
 * Copyright (C) 2013-2014 Pier Luigi Fiorini <pierluigi.fiorini@gmail.com>
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

#ifndef SHELLUI_H
#define SHELLUI_H

#include <QtCore/QObject>
#include <QtGui/QScreen>

#include "scriptengine.h"

class DesktopView;
class GrabWindow;
class LockScreenView;
class PanelView;

class ShellUi : public QObject
{
    Q_OBJECT
public:
    ShellUi(QObject *parent = 0);
    ~ShellUi();

    ScriptEngine *jsEngine() const;

    GrabWindow *grabWindow() const;

    LockScreenView *lockScreenView() const;

    QList<DesktopView *> desktops() const;
    QList<PanelView *> panels() const;

    void load();
    void unload();

public Q_SLOTS:
    void createLockScreen();
    void closeLockScreen();

    void setGrabCursor(const QCursor &cursor);

    void setNumWorkspaces(int num);

    void setShell(const QString &shell);
    void setLookAndFeel(const QString &lookAndFeel);

private:
    ScriptEngine *m_jsEngine;
    int m_numWorkspaces;
    GrabWindow *m_grabWindow;
    LockScreenView *m_lockScreenView;
    QList<DesktopView *> m_desktopViews;
    QList<PanelView *> m_panelViews;
    QString m_shell;
    QString m_lookAndFeel;

    void synchronize();

private Q_SLOTS:
    void screenAdded(QScreen *screen);
    void screenDestroyed(QObject *object);

    void printScriptMessage(const QString &text);
    void printScriptError(const QString &text);
};

#endif // SHELLUI_H
