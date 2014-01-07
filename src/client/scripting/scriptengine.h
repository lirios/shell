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

#ifndef SCRIPTENGINE_H
#define SCRIPTENGINE_H

#include <QtScript/QScriptEngine>

static const int HAWAII_SHELL_SCRIPTING_VERSION = 1;

class AppInterface;
class ShellUi;

class ScriptEngine : public QScriptEngine
{
    Q_OBJECT
public:
    explicit ScriptEngine(QObject *parent = 0);

    bool evaluateScript(const QString &script, const QString &fileName = QString());

    static ScriptEngine *envFor(QScriptEngine *engine);
    static AppInterface *interfaceFor(QScriptEngine *engine);
    static ShellUi *uiFor(QScriptEngine *engine);

Q_SIGNALS:
    void print(const QString &text);
    void printError(const QString &text);

private:
    QScriptValue m_self;

    void setupEngine();

    QScriptValue wrapWithScriptOwnership(QObject *object);
    QScriptValue wrapWithQtOwnership(QObject *object);

    static QScriptValue newPanel(QScriptContext *context, QScriptEngine *engine);
    static QScriptValue panels(QScriptContext *context, QScriptEngine *engine);

private Q_SLOTS:
    void exception(const QScriptValue &value);
};

#endif // SCRIPTENGINE_H
